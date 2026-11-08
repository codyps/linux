#!/usr/bin/env python3
# SPDX-License-Identifier: GPL-2.0-only
"""Canonicalize one verified compiler-private TG3040 IOMMU clone identity.

The maintained C produces the shipped 516-byte implementation, but this GCC
assigns .isra.2 instead of the OEM .isra.4. Original IPA numbering is unknown.
Normalize the native local symbol before linking; kernel placement is irrelevant.
No instruction, initialized byte, section or runtime relocation may change.
"""
import argparse
from collections import Counter
from hashlib import sha256
import json,shlex,struct,subprocess
from pathlib import Path
OLD='__full_va_sweep.isra.2';NEW='__full_va_sweep.isra.4'
EXPECTED_CODE_SHA='ea6f1bab46266e8006bb53f3ce71891af1b37a097237ca7b67916310952b25d7'
EXPECTED_RELA_SHA='d152b03ae88ff2bc265b3113533ba9a5612ed7275527cbdf1ac49ee80c13d989'

def require(condition,message):
    if not condition:raise ValueError(message)
def canonical(name):return NEW if name==OLD else name
def fingerprint(rows):return sha256(json.dumps(sorted(rows),separators=(',',':')).encode()).hexdigest()

class Object:
    def __init__(self,path):
        self.raw=Path(path).read_bytes();raw=self.raw
        require(len(raw)>=64 and raw[:6]==b'\x7fELF\x02\x01','expected ELF64 little-endian object')
        header=struct.unpack_from('<16sHHIQQQIHHHHHH',raw)
        require(header[1:3]==(1,183) and header[11]==64,'expected AArch64 relocatable object')
        start,count,names=header[6],header[12],header[13]
        require(count>0 and names<count and start+count*64<=len(raw),'invalid section table')
        self.sections=[struct.unpack_from('<IIQQQQIIQQ',raw,start+i*64) for i in range(count)]
        strings=self.data(names)
        self.names=[self.string(strings,s[0]) for s in self.sections]
        require(self.names.count('.symtab')==1,'missing/ambiguous symbol table')
        symidx=self.names.index('.symtab');symsec=self.sections[symidx];require(symsec[9]==24,'unexpected symbol width')
        strings=self.data(symsec[6]);self.symbols=[]
        for at in range(0,symsec[5],24):
            name,info,other,index,value,size=struct.unpack_from('<IBBHQQ',self.data(symidx),at)
            self.symbols.append((self.string(strings,name),info,other,index,value,size))
    @staticmethod
    def string(data,offset):
        require(0<=offset<len(data),'string offset out of range')
        return data[offset:data.index(0,offset)].decode('ascii')
    def data(self,index):
        require(index<len(self.sections),'section index out of range');s=self.sections[index]
        require(s[1]==8 or s[4]+s[5]<=len(self.raw),'section exceeds object')
        return b'' if s[1]==8 else self.raw[s[4]:s[4]+s[5]]
    def section(self,index):return self.names[index] if 0<index<len(self.names) else 'RESERVED:'+str(index)
    def relocations(self):
        rows=[]
        for index,s in enumerate(self.sections):
            if s[1]!=4 or not self.sections[s[7]][2]&2:continue
            require(s[9]==24 and self.names[s[6]]=='.symtab','unexpected relocation table')
            for at in range(0,s[5],24):
                offset,info,addend=struct.unpack_from('<QQq',self.data(index),at)
                name,bind_type,other,section,value,size=self.symbols[info>>32]
                rows.append((self.names[s[7]],offset,info&0xffffffff,canonical(name),bind_type,other,self.section(section),value,size,addend))
        return rows
    def native_identity(self):
        entries=[s for s in self.symbols if s[0] in (OLD,NEW)]
        require(len(entries)==1,'expected exactly one known private clone; refuse unknown/multiple identities')
        name,info,other,index,value,size=entries[0]
        require(info==2 and other==0 and size==516 and self.names[index]=='.text','unexpected private clone binding/type/size/section')
        offset=value-self.sections[index][3];code=self.data(index)[offset:offset+size]
        require(offset>=0 and len(code)==516,'clone extent out of range')
        rows=[(r[1]-value,*r[2:]) for r in self.relocations() if r[0]=='.text' and value<=r[1]<value+size]
        return name,sha256(code).hexdigest(),fingerprint(rows)
    def snapshot(self):
        allocated={name:(s[1],s[2],s[3],s[5],s[8],None if s[1]==8 else sha256(self.data(i)).hexdigest()) for i,(name,s) in enumerate(zip(self.names,self.sections)) if s[2]&2}
        symbols=Counter((canonical(n),t,o,self.section(i),v,z) for n,t,o,i,v,z in self.symbols)
        return allocated,symbols,Counter(self.relocations())

def normalize(path,objcopy):
    before=Object(path);name,code,edges=before.native_identity()
    require(code==EXPECTED_CODE_SHA and edges==EXPECTED_RELA_SHA,'clone code/relocation fingerprint changed; review reconstruction before naming')
    snapshot=before.snapshot()
    if name==OLD:subprocess.run([*shlex.split(objcopy),'--redefine-sym',OLD+'='+NEW,str(path)],check=True)
    after=Object(path);require(after.native_identity()==(NEW,code,edges),'canonical name/identity verification failed')
    require(after.snapshot()==snapshot,'allocated bytes/symbols/runtime relocations changed')
    return name==OLD

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--object',required=True);parser.add_argument('--objcopy',required=True);args=parser.parse_args()
    normalize(args.object,args.objcopy)

if __name__=='__main__':main()
