/* SPDX-License-Identifier: GPL-2.0 */
/* TG3040 #860 NFTL readonly pool recovered from the finished Image.
 * The 504 strings, CRC16 table, and two Hynix parameter tables retain
 * instruction-rooted stock offsets. Local diagnostic field names describe
 * observed assembly labels; original C declarations remain unknown.
 */

struct nftl_readonly_layout {
	char com[16];
	char mod[8];
	char lc_9[32];
	char lc_10[8];
	char lc_11[16];
	char lc_12[8];
	char lc_13[8];
	char lc_14[8];
	char lc_15[40];
	char lc_16[40];
	char lc_17[32];
	char lc_0[16];
	char lc_3[32];
	char lc_4[40];
	char lc_5[32];
	char lc_6[24];
	char lc_7[24];
	char lc_8[32];
	char lc_400[24];
	char lc_401[32];
	char lc_402[16];
	char lc_403[16];
	char lc_404[32];
	char lc_405[32];
	char lc_406[32];
	char lc_407[24];
	char lc_408[32];
	char lc_409[24];
	char lc_410[32];
	char lc_411[64];
	char lc_412[56];
	char lc_45[8];
	char lc_200[8];
	char lc_122[8];
	char lc_201[48];
	char lc_202[40];
	char lc_203[40];
	char lc_204[32];
	char lc_205[56];
	char lc_206[56];
	char lc_207[56];
	char lc_208[24];
	char lc_209[32];
	char lc_210[24];
	char lc_211[16];
	char lc_212[40];
	char lc_213[40];
	char lc_214[16];
	char lc_215[16];
	char lc_216[24];
	char lc_217[24];
	char lc_218[32];
	char lc_219[24];
	char lc_220[88];
	char lc_221[24];
	char lc_222[32];
	char lc_223[32];
	char lc_224[24];
	char lc_225[32];
	char lc_226[32];
	char lc_227[48];
	char lc_228[80];
	char lc_229[24];
	char lc_2[8];
	char lc_230[32];
	char lc_231[88];
	char lc_232[48];
	char lc_233[56];
	char lc_234[56];
	char lc_235[56];
	char lc_236[56];
	char lc_237[56];
	char lc_238[88];
	char lc_239[48];
	char lc_240[40];
	char lc_241[32];
	char lc_242[24];
	char lc_243[40];
	char lc_244[80];
	char lc_245[80];
	char lc_246[32];
	char lc_247[40];
	char lc_248[40];
	char lc_249[72];
	char lc_250[40];
	char lc_251[32];
	char lc_252[24];
	char lc_253[48];
	char lc_254[48];
	char lc_255[40];
	char lc_256[40];
	char lc_257[88];
	char lc_258[88];
	char lc_259[40];
	char lc_260[32];
	char lc_261[32];
	char lc_262[56];
	char lc_263[40];
	char lc_264[64];
	char lc_265[32];
	char lc_266[32];
	char lc_267[56];
	char lc_268[64];
	char lc_269[56];
	char lc_270[56];
	char lc_271[32];
	char lc_272[56];
	char lc_273[32];
	char lc_274[56];
	char lc_275[64];
	char lc_276[56];
	char lc_277[48];
	char lc_278[48];
	char lc_279[16];
	char lc_280[16];
	char lc_281[16];
	char lc_282[16];
	char lc_283[40];
	char lc_284[48];
	char lc_285[56];
	char lc_286[48];
	char lc_287[64];
	char lc_288[48];
	char lc_289[40];
	char lc_290[40];
	char lc_291[32];
	char lc_292[40];
	char lc_293[24];
	char lc_294[24];
	char lc_295[48];
	char lc_296[32];
	char lc_297[48];
	char lc_298[64];
	char lc_299[32];
	char lc_300[32];
	char lc_301[24];
	char lc_302[32];
	char lc_303[32];
	char lc_304[56];
	char lc_305[56];
	char lc_306[40];
	char lc_307[56];
	char lc_308[32];
	char lc_309[104];
	char lc_310[32];
	char lc_311[32];
	char lc_312[64];
	char lc_313[48];
	char lc_314[40];
	char lc_315[32];
	char lc_316[40];
	char lc_317[40];
	char lc_318[56];
	char lc_319[56];
	char lc_320[48];
	char lc_321[40];
	char lc_322[24];
	char lc_323[24];
	char lc_324[56];
	char lc_325[48];
	char lc_326[40];
	char lc_327[32];
	char lc_328[32];
	char lc_329[16];
	char lc_330[40];
	char lc_331[48];
	char lc_332[48];
	char lc_333[48];
	char lc_334[48];
	char lc_335[24];
	char lc_336[24];
	char lc_337[32];
	char lc_338[24];
	char lc_339[32];
	char lc_340[40];
	char lc_341[40];
	char lc_342[32];
	char lc_343[56];
	char lc_344[48];
	char lc_345[40];
	char lc_346[40];
	char lc_347[40];
	char lc_348[64];
	char lc_349[56];
	char lc_350[48];
	char lc_351[40];
	char lc_352[48];
	char lc_353[64];
	char lc_354[40];
	char lc_355[64];
	char lc_356[40];
	char lc_357[40];
	char lc_358[40];
	char lc_359[64];
	char lc_360[40];
	char lc_361[64];
	char lc_362[40];
	char lc_363[32];
	char lc_364[32];
	char lc_365[40];
	char lc_366[32];
	char lc_367[32];
	char lc_368[48];
	char lc_369[48];
	char lc_370[56];
	char lc_371[48];
	char lc_372[40];
	char lc_373[48];
	char lc_374[56];
	char lc_375[48];
	char lc_376[56];
	char lc_377[64];
	char lc_378[40];
	char lc_379[48];
	char lc_380[40];
	char lc_381[48];
	char lc_382[32];
	char lc_383[64];
	char lc_384[48];
	char lc_385[32];
	char lc_386[32];
	char lc_387[32];
	char lc_388[32];
	char lc_389[32];
	char lc_390[80];
	char lc_391[32];
	char lc_392[32];
	char lc_393[32];
	char lc_394[8];
	char lc_395[32];
	char lc_396[24];
	char lc_397[24];
	char lc_398[24];
	char lc_399[16];
	char lc_141[40];
	char lc_142[32];
	char lc_143[32];
	char lc_144[32];
	char lc_145[40];
	char lc_470[48];
	char lc_471[48];
	char lc_472[48];
	char lc_473[56];
	char lc_176[24];
	char lc_177[48];
	char lc_178[48];
	char lc_179[40];
	char lc_180[48];
	char lc_181[48];
	char lc_182[48];
	char lc_183[48];
	char lc_184[32];
	char lc_185[32];
	char lc_186[32];
	char lc_187[40];
	char lc_188[48];
	char lc_189[48];
	char lc_190[48];
	char lc_191[48];
	char lc_192[40];
	char lc_193[40];
	char lc_194[40];
	char lc_195[40];
	char lc_196[40];
	char lc_197[40];
	char lc_198[40];
	char lc_199[40];
	unsigned short nand_crc16_table[256];
	char lc_146[56];
	char lc_147[32];
	char lc_148[40];
	char lc_149[24];
	char lc_150[40];
	char lc_151[56];
	char lc_152[32];
	char lc_153[32];
	char lc_154[32];
	char lc_155[32];
	char lc_156[40];
	char lc_157[40];
	char lc_158[48];
	char lc_159[40];
	char lc_160[40];
	char lc_161[48];
	char lc_162[48];
	char lc_163[56];
	char lc_164[56];
	char lc_165[32];
	char lc_166[48];
	char lc_167[32];
	char lc_168[96];
	char lc_169[48];
	char lc_170[96];
	char lc_171[48];
	char lc_172[72];
	char lc_173[24];
	char lc_174[48];
	char lc_175[56];
	char lc_413[40];
	char lc_414[40];
	char lc_415[32];
	char lc_416[32];
	char lc_417[40];
	char lc_418[48];
	char lc_419[48];
	char lc_420[32];
	char lc_421[24];
	char lc_422[40];
	char lc_423[40];
	char lc_424[40];
	char lc_425[40];
	char lc_426[48];
	char lc_427[48];
	char lc_428[40];
	char lc_429[24];
	char lc_430[40];
	char lc_431[24];
	char lc_432[24];
	char lc_474[32];
	char lc_475[40];
	char lc_476[48];
	char lc_477[32];
	char lc_478[24];
	char lc_479[32];
	char lc_480[40];
	char lc_481[32];
	char lc_482[16];
	char lc_433[40];
	char lc_434[40];
	char lc_435[40];
	char lc_436[40];
	char lc_437[40];
	char lc_438[72];
	char lc_439[104];
	char lc_440[48];
	char lc_441[64];
	char lc_442[56];
	char lc_443[80];
	char lc_444[56];
	char lc_445[48];
	char lc_446[32];
	char lc_447[32];
	char lc_448[32];
	char lc_449[56];
	char lc_450[32];
	char lc_451[32];
	char lc_452[40];
	char lc_453[32];
	char lc_454[32];
	char lc_455[64];
	char lc_456[72];
	char lc_457[40];
	char lc_458[32];
	char lc_459[48];
	char lc_460[24];
	char lc_461[40];
	char lc_462[48];
	char lc_463[32];
	char lc_464[24];
	char lc_465[24];
	char lc_466[40];
	char lc_467[40];
	char lc_468[32];
	char lc_469[16];
	char lc_18[40];
	char lc_19[40];
	char lc_20[32];
	char lc_21[40];
	char lc_22[80];
	char lc_23[64];
	char lc_24[48];
	char lc_25[48];
	char lc_26[32];
	char lc_27[40];
	char lc_28[48];
	char lc_29[40];
	char lc_30[32];
	char lc_31[48];
	char lc_32[32];
	char lc_33[48];
	char lc_34[48];
	char lc_35[24];
	char lc_107[32];
	char lc_108[48];
	char lc_109[32];
	char lc_110[16];
	char lc_111[80];
	char lc_104[80];
	char lc_112[24];
	char lc_113[56];
	char lc_114[24];
	char lc_115[24];
	char lc_116[56];
	char lc_117[16];
	char lc_118[56];
	char lc_119[16];
	char lc_120[64];
	char lc_121[72];
	char lc_123[56];
	char lc_124[32];
	char lc_51[48];
	char lc_125[48];
	char lc_126[40];
	char lc_127[64];
	char lc_54[40];
	char lc_56[32];
	char lc_57[8];
	char lc_59[48];
	char lc_128[56];
	char lc_61[8];
	char lc_62[88];
	char lc_129[48];
	char lc_64[56];
	char lc_130[32];
	char lc_131[120];
	char lc_132[128];
	char lc_133[32];
	char lc_36[40];
	char lc_37[16];
	char lc_38[88];
	char lc_39[32];
	char lc_40[80];
	char lc_41[88];
	char lc_42[24];
	unsigned short para0[28];
	unsigned short para1[28];
	char lc_85[24];
	char lc_44[8];
	char lc_86[16];
	char lc_87[88];
	char lc_88[24];
	char lc_89[80];
	char lc_90[32];
	char lc_91[120];
	char lc_92[128];
	char lc_93[32];
	char lc_94[24];
	char lc_95[40];
	char lc_47[88];
	char lc_96[88];
	char lc_97[104];
	char lc_98[40];
	char lc_99[40];
	char lc_100[128];
	char lc_101[136];
	char lc_102[56];
	char lc_103[32];
	char lc_105[24];
	char lc_106[24];
	char lc_77[40];
	char lc_78[40];
	char lc_79[16];
	char lc_80[88];
	char lc_81[32];
	char lc_82[112];
	char lc_83[120];
	char lc_84[24];
	char lc_69[64];
	char lc_70[16];
	char lc_71[88];
	char lc_72[40];
	char lc_73[32];
	char lc_74[80];
	char lc_75[88];
	char lc_76[24];
	char lc_134[16];
	char lc_135[88];
	char lc_136[40];
	char lc_1[16];
	char lc_137[32];
	char lc_138[112];
	char lc_139[120];
	char lc_140[24];
	char lc_43[16];
	char lc_46[24];
	char lc_48[24];
	char lc_49[88];
	char lc_50[24];
	char lc_52[24];
	char lc_53[24];
	char lc_55[40];
	char lc_58[40];
	char lc_60[56];
	char lc_63[40];
	char lc_65[32];
	char lc_66[120];
	char lc_67[128];
	char lc_68[32];
	char func_5346[16];
	char func_5194[16];
	char func_7098[16];
	char func_7312[16];
	char func_7402[24];
	char func_7526[32];
	char func_7558[24];
	char func_7569[24];
	char func_7580[24];
	char func_7589[24];
	char func_7642[24];
	char function_7666[32];
	char func_6755[24];
	char func_8479[24];
	char func_7884[16];
	char func_8536[16];
	char func_8331[16];
	char func_5524[24];
	char func_6409[25];
} __attribute__((packed));

static const struct nftl_readonly_layout nftl_readonly_storage
	__asm__(".Lnftl_readonly_storage")
	__attribute__((used, aligned(8))) = {
	.com = "compatible",
	.mod = "model",
	.lc_9 = "[NE]malloc size too large %d!\n",
	.lc_10 = "09:50",
	.lc_11 = "2020.12.24",
	.lc_12 = "000",
	.lc_13 = "0",
	.lc_14 = "5",
	.lc_15 = "sunxi nftl version %s.%s.%s  %s-%s\n",
	.lc_16 = "[NE]%s:malloc fail for nftl_cfg\n",
	.lc_17 = "[ND]max_erase_times = %i\n",
	.lc_0 = "allwinner",
	.lc_3 = "[NE]nftl_build_all error!\n",
	.lc_4 = "[NE]%s:malloc fail for nftl_blk\n",
	.lc_5 = "[NE]nftl_initialize failed\n",
	.lc_6 = "[ND]nftl_add ok\n",
	.lc_7 = "[ND]cap: 0x%08x.\n",
	.lc_8 = "[NE]parameter error %d,%d !\n",
	.lc_400 = "[ND]free list: \n",
	.lc_401 = "[ND]block NO:%4d; erase:%3d; ",
	.lc_402 = "[ND]count:%4d; ",
	.lc_403 = "[ND]info:0x%x\n",
	.lc_404 = "[ND]invalid_page_count list: \n",
	.lc_405 = "[ND]count:%4d; invalid:%3d;",
	.lc_406 = "[ND]block_used_count list: \n",
	.lc_407 = "[ND]block NO: %d ",
	.lc_408 = "[ND]block_used_count: %d \n",
	.lc_409 = "[ND]free_list error !\n",
	.lc_410 = "[ND]add invalid_list error !\n",
	.lc_411 = "[ND]invalid_page_count more than pages per blk 0 %d %d!!\n",
	.lc_412 = "[ND]invalid_page_count more than pages per blk 1!!\n",
	.lc_45 = "\n",
	.lc_200 = "%08x: ",
	.lc_122 = "%02x ",
	.lc_201 = "[NE]exception case wb %04x %04x, len: %d\n",
	.lc_202 = "[NE]%s:malloc fail for temp_page_buf\n",
	.lc_203 = "[NE]%s:malloc fail for logic_page_buf\n",
	.lc_204 = "[NE]zone_param_init error2\n",
	.lc_205 = "[NE]%s:malloc fail for current_block.user_info.buf\n",
	.lc_206 = "[NE]%s:malloc fail for assist_block.user_info.buf\n",
	.lc_207 = "[NE]%s:malloc fail for zone_phy_page_map_for_gc.buf\n",
	.lc_208 = "[NE]mtd size changed !\n",
	.lc_209 = "[ND]build_zone_list_first.\n",
	.lc_210 = "[NE]no block %d\n",
	.lc_211 = "[ND]first\n",
	.lc_212 = "[NE]init_zone_after_first_scan error\n",
	.lc_213 = "[NE]second_scan_all_blocks error\n",
	.lc_214 = "[NI]nftl ok!\n",
	.lc_215 = "[ND]fsab\n",
	.lc_216 = "[NI]ftl end block! %d!\n",
	.lc_217 = "[ND]READ_RECLAIM!\n",
	.lc_218 = "[NI]start page , do nothing\n",
	.lc_219 = "[ND]fill block %d\n",
	.lc_220 = "[NI]b@%d p@0 unknow spare data:%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x.\n",
	.lc_221 = "[NE]nftl bad block: %d\n",
	.lc_222 = "[NE]nftl b@%d p@0 ecc error\n",
	.lc_223 = "[NE]b@%d last page ok !\n",
	.lc_224 = "[NI]READ_RECLAIM!\n",
	.lc_225 = "[NE]last page blank ! %d!\n",
	.lc_226 = "[NE]last page unkown ! %d!\n",
	.lc_227 = "[NE]b@%d first page last page ecc error\n",
	.lc_228 = "[NE]b@%d last page spare data %x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x,%x\n",
	.lc_229 = "[NI]nftl bad block: %d\n",
	.lc_2 = "sunxi",
	.lc_230 = "[ND]before second %d %d.\n",
	.lc_231 = "[NE]only have blocks:%u,logic cap need blocks:%u this zone not enough data block!!\n",
	.lc_232 = "[NE]this zone not enough free block %d , %d!!\n",
	.lc_233 = "[NI]NAND_EVENT: 2 current used block found %d,%d!\n",
	.lc_234 = "[NE]no crosstalk:2 block used count diff %d,%d %d,%d !\n",
	.lc_235 = "[NE]NAND_EVENT: muti current used block found1 %d %d!\n",
	.lc_236 = "[NE]NAND_EVENT: muti current used block found2 %d %d!\n",
	.lc_237 = "[NE]NAND_EVENT: muti current used block found3 %d %d!\n",
	.lc_238 = "[NE]NAND_EVENT: last page spare data %x,%x,%x,%x,%x,%x,%x %x %x %x %x %x %x %x %x %x!!\n",
	.lc_239 = "[NI]zone@%d b@%d last page unkown ecc ok.\n",
	.lc_240 = "[NE]zone@%d b@%d last page ecc error.\n",
	.lc_241 = "[NE]something strange %d,%d!!\n",
	.lc_242 = "[NI]all block full!!\n",
	.lc_243 = "[ND]corss talk rebuild 0 %d %d!!\n",
	.lc_244 = "[NE]1st current block:%d 1st current block uc:%d muti current used block 1st.\n",
	.lc_245 = "[ND]2nd current block:%d 2nd current block uc:%d muti current used block 2nd.\n",
	.lc_246 = "[NE]something is error here 7!\n",
	.lc_247 = "[NE]p_phy_block_info_last %d %d.\n",
	.lc_248 = "[NE]get_current_used_block fail.\n",
	.lc_249 = "[ND]free block nums is %d, aec: %d, iec: %d, a_buc: %d, i_buc: %d!\n",
	.lc_250 = "[NE]current_block ecc error %d!\n",
	.lc_251 = "[NE]error! no free block!\n",
	.lc_252 = "[ND]recover %d %d\n",
	.lc_253 = "[NE]page used:%d invalid page count:%d error\n",
	.lc_254 = "[NE]new_block_init_for_write error here!\n",
	.lc_255 = "[NE]something is error here 10!\n",
	.lc_256 = "[NE]something is error here 11!\n",
	.lc_257 = "[NE]assist block page used:%d current block page used:%d something is error here 12.\n",
	.lc_258 = "[NE]assist block page used:%d current block page used:%d something is error here 17.\n",
	.lc_259 = "[NE]something is error here 16!\n",
	.lc_260 = "[ND]get a new free block\n",
	.lc_261 = "[NE]no free block to use!\n",
	.lc_262 = "[NE]sb@%d sp@%d recover_phy_page_mapping no used page\n",
	.lc_263 = "[NE]sb@%d sp@%d power off ecc error1.\n",
	.lc_264 = "[NE]invalid logic_page block:%d page:%d,logic_page 0x%x.\n",
	.lc_265 = "[NE]rppm invalid page %d!!\n",
	.lc_266 = "[NI]power_down page %d!!\n",
	.lc_267 = "[NE]recover_phy_page_mapping unkown page data %d!!\n",
	.lc_268 = "[NE]recover_phy_page_mapping ecc error block:%d;page:%d.\n",
	.lc_269 = "[NE]ret:%d read fail %d %d,read one more time!!\n",
	.lc_270 = "[NE]ret:%d read fail %d %d,%d times,don't read more!!\n",
	.lc_271 = "[NE]power off ecc error 3!!\n",
	.lc_272 = "[NE]recover sb@%d sp@%d phy page mapping error.\n",
	.lc_273 = "[NI]logic_page_map is NULL\n",
	.lc_274 = "[NE]invalid_page_count more than pages_per_blk :%d\n",
	.lc_275 = "[NE]invalid_page data block:%d page:%d,logic_page 0x%x!!\n",
	.lc_276 = "[NE]build invalid_page_count more than pages_per_blk!!\n",
	.lc_277 = "[NE]NAND EVENT:do_write_error erase error1!\n",
	.lc_278 = "[NE]NAND EVENT:do_write_error erase error2!\n",
	.lc_279 = "%s %d:fail 1\n",
	.lc_280 = "%s %d:fail 2\n",
	.lc_281 = "%s %d:fail 3\n",
	.lc_282 = "%s %d:fail 4\n",
	.lc_283 = "[NE]pages_per_blk: %d, %p, %p, %p, %p\n",
	.lc_284 = "[NE]cross_talk_rebuild_current_block fail 1\n",
	.lc_285 = "[NE]cross_talk_rebuild_current_block!! %d,%d,%d!\n",
	.lc_286 = "[NE]cross_talk_rebuild_current_block fail 2\n",
	.lc_287 = "[NE]cross_talk_rebuild_current_block!! %d,%d,%d,%d,%d,%d!\n",
	.lc_288 = "[NE]cross_talk_rebuild_current_block fail 3\n",
	.lc_289 = "[NE]corss talk rebuild 1 %d %d %d!\n",
	.lc_290 = "[NE]corss talk rebuild 2 %d %d %d!\n",
	.lc_291 = "[NE]get free block fail!\n",
	.lc_292 = "[NE]something is error here 14!\n",
	.lc_293 = "[NE]page lost %d %d\n",
	.lc_294 = "[NI]nctrcb1 %d,%d,%d!\n",
	.lc_295 = "[NE]no_cross_talk_rebuild_current_block fail 2\n",
	.lc_296 = "[NI]nctrcb2 %d,%d,%d,%d,%d,%d!\n",
	.lc_297 = "[NE]no_cross_talk_rebuild_current_block fail 4\n",
	.lc_298 = "[NE]no_cross_talk_rebuild_current_block erase block fail %d!\n",
	.lc_299 = "[NI]erase_block block %d!\n",
	.lc_300 = "[NE]erase_block block fail %d!\n",
	.lc_301 = "[NI]torture b@%d.\n",
	.lc_302 = "[NE]%s write b@%d p@%d err\n",
	.lc_303 = "[NE]%s read b@%d p@%d err\n",
	.lc_304 = "[NE]%s b@%d p@%d patt %02x checking main data fail\n",
	.lc_305 = "[NE]%s b@%d p@%d patt %02x checking spare data fail\n",
	.lc_306 = "[NE][NE]erase_block block fail %d!\n",
	.lc_307 = "[NE]get_used_page_num ecc error block:%d page:%d!\n",
	.lc_308 = "[NE]power off ecc error 4!!\n",
	.lc_309 = "[NE]sb@%d sp@%d get_used_page_num used spare data %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
	.lc_310 = "[NE]gupnnc ecc err b:%d p:%d!\n",
	.lc_311 = "[NE]gupnnc power off ecc err!!\n",
	.lc_312 = "[NE]cross talk block:%d total_good_num %d good_page_num: %d!\n",
	.lc_313 = "[NE]current_block_ecc_error no free block!!!\n",
	.lc_314 = "[NE]do current_block_ecc_error %d!!\n",
	.lc_315 = "[NE]power off ecc error 2.\n",
	.lc_316 = "[NE]current_block_ecc ok page %d %d!!\n",
	.lc_317 = "[NE]current_block_ecc_error page %d!!\n",
	.lc_318 = "[NE]current_block_ecc_error invalid page data %d!!\n",
	.lc_319 = "[NE]current_block_ecc_error unkown page data %d!!\n",
	.lc_320 = "[ND]do current_block_ecc_error end %d %d!!\n",
	.lc_321 = "[NE]do block_last_page_ecc_error!!\n",
	.lc_322 = "[NE]%s:malloc fail 1!\n",
	.lc_323 = "[NE]%s:malloc fail 2!\n",
	.lc_324 = "[NE]current_block_ecc_error invalid page data!!\n",
	.lc_325 = "[NE]current_block_ecc_error unkown page data!!\n",
	.lc_326 = "[NE]do block_last_page_ecc_error end!!\n",
	.lc_327 = "[NE]adjust free list fail!!\n",
	.lc_328 = "[NI]adjust free list : %d %d!!\n",
	.lc_329 = "[ND]nand3.x \n",
	.lc_330 = "[ND]zone->nand_chip->blk_per_chip: %d \n",
	.lc_331 = "[ND]zone->nand_chip->bytes_per_page: %d \n",
	.lc_332 = "[ND]zone->nand_chip->pages_per_blk: %d \n",
	.lc_333 = "[ND]zone->nand_chip->max_erase_times: %d \n",
	.lc_334 = "[ND]zone->nand_chip->support_read_reclaim: %d \n",
	.lc_335 = "[ND]zone->test: %d \n",
	.lc_336 = "[ND]zone->zone_no: %d \n",
	.lc_337 = "[ND]zone->zone_attr: %d \n",
	.lc_338 = "[ND]zone->blocks: %d \n",
	.lc_339 = "[ND]zone->bad_block: %d \n",
	.lc_340 = "[ND]zone->logic_cap_in_sects: %d \n",
	.lc_341 = "[ND]zone->backup_cap_in_sects: %d \n",
	.lc_342 = "[ND]zone->free_block_num: %d \n",
	.lc_343 = "[ND]zone->gc_strategy.start_gc_free_blocks: %d \n",
	.lc_344 = "[ND]zone->gc_strategy.stop_gc_free_blocks: %d \n",
	.lc_345 = "[ND]zone->gc_strategy.gc_page: %d \n",
	.lc_346 = "[ND]zone->gc_strategy.process: %d \n",
	.lc_347 = "[ND]zone->prio_gc.prio_type : %d \n",
	.lc_348 = "[ND]zone->zone_start_phy_block->phy_block.Block_NO: %d \n",
	.lc_349 = "[ND]zone->zone_end_phy_block->phy_block.Block_NO: %d \n",
	.lc_350 = "[ND]zone->zone_phy_page_map_for_gc: %x \n",
	.lc_351 = "[ND]zone->current_block.user_info: %x \n",
	.lc_352 = "[ND]zone->current_block.block_info: %x \n",
	.lc_353 = "[ND]zone->current_block.block_info->phy_block.Block_NO: %d \n",
	.lc_354 = "[ND]zone->current_block.page_used: %d \n",
	.lc_355 = "[ND]zone->current_block.block_info->block_used_count: %d \n",
	.lc_356 = "[ND]zone->recv_free_block: %d %d!!\n",
	.lc_357 = "[ND]zone->assist_block.user_info: %x \n",
	.lc_358 = "[ND]zone->assist_block.block_info: %x \n",
	.lc_359 = "[ND]zone->assist_block.block_info->phy_block.Block_NO: %d \n",
	.lc_360 = "[ND]zone->assist_block.page_used: %d \n",
	.lc_361 = "[ND]zone->assist_block.block_info->block_used_count: %d \n",
	.lc_362 = "[ND]zone->read_reclaim_complete: %x \n",
	.lc_363 = "[ND]zone->temp_page_buf: %x \n",
	.lc_364 = "[ND]zone->max_erase_num: %d \n",
	.lc_365 = "[ND]zone->cache.cache_totals: %x \n",
	.lc_366 = "[ND]zone->wl_gc_count: %d \n",
	.lc_367 = "[ND]zone->wl_gc_page_num: %d \n",
	.lc_368 = "[ND]zone->cfg->nftl_dont_use_cache: %x \n",
	.lc_369 = "[ND]zone->cfg->nftl_use_cache_sort: %x \n",
	.lc_370 = "[ND]zone->cfg->nftl_support_gc_read_reclaim: %x \n",
	.lc_371 = "[ND]zone->cfg->nftl_support_wear_leveling: %x \n",
	.lc_372 = "[ND]zone->cfg->nftl_need_erase: %x \n",
	.lc_373 = "[ND]zone->cfg->nftl_min_free_block_num: %x \n",
	.lc_374 = "[ND]zone->cfg->nftl_gc_threshold_free_block_num: %x \n",
	.lc_375 = "[ND]zone->cfg->nftl_min_free_block: %x \n",
	.lc_376 = "[ND]zone->cfg->nftl_gc_threshold_ratio_numerator: %x \n",
	.lc_377 = "[ND]zone->cfg->nftl_gc_threshold_ratio_denominator: %x \n",
	.lc_378 = "[ND]zone->cfg->nftl_max_cache_num: %d \n",
	.lc_379 = "[ND]zone->cfg->nftl_max_cache_write_num: %d \n",
	.lc_380 = "[ND]zone->cfg->nftl_cross_talk: %x \n",
	.lc_381 = "[ND]zone->cfg->nftl_read_claim_interval: %d \n",
	.lc_382 = "[NE]%s:malloc 4096 fail!\n",
	.lc_383 = "[ND]spare: %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x",
	.lc_384 = "[ND]buf:%x %x %x %x %x %x %x %x %x %x %x.\n",
	.lc_385 = "[NE]%s:malloc 8192 fail!\n",
	.lc_386 = "[ND]zoneNO:%d total_pages: %d\n",
	.lc_387 = "[ND]nand_memset bytes: %d\n",
	.lc_388 = "[ND]build_zone_list start\n",
	.lc_389 = "[ND]nftl_cache_init start\n",
	.lc_390 = "[ND]=================================2=======================================\n",
	.lc_391 = "[ND]nand_find_zone_table end\n",
	.lc_392 = "[NE][NE]%s:malloc 512 fail!\n",
	.lc_393 = "[ND]==========================\n",
	.lc_394 = "%.2x ",
	.lc_395 = "[NE]exception case bad block\n",
	.lc_396 = "[NE]exception case wbc\n",
	.lc_397 = "[NE]exception case gd\n",
	.lc_398 = "[NE]%s %d, flag = %d\n",
	.lc_399 = "[NE]OOB :",
	.lc_141 = "[NE]%s:malloc fail for cache_buf!\n",
	.lc_142 = "[NE]nand_sector_cache_read 1!\n",
	.lc_143 = "[NE]nand_sector_cache_write 1!\n",
	.lc_144 = "[NE]nand_sector_cache_write 2!\n",
	.lc_145 = "[NE]nand_flush_write_cache write error\n",
	.lc_470 = "[NE]%s:logic_page:0x%x >= total_pages:0x%x\n",
	.lc_471 = "[NE]%s:malloc fail for logic_page_map %d\n",
	.lc_472 = "[NE]%s: malloc fail for logic_page_map %d\n",
	.lc_473 = "[NE]zone_param_init malloc_logic_page_map error %d\n",
	.lc_176 = "[ND]smart->version:%x;\n",
	.lc_177 = "[ND]smart->total_recv_read_sectors:%lld;\n",
	.lc_178 = "[ND]smart->total_recv_write_sectors:%lld;\n",
	.lc_179 = "[ND]smart->total_real_read_pages:%lld;\n",
	.lc_180 = "[ND]smart->total_real_write_pages:%lld;\n",
	.lc_181 = "[ND]smart->total_recv_discard_sectors:%lld;\n",
	.lc_182 = "[ND]smart->total_real_discard_sectors:%lld;\n",
	.lc_183 = "[ND]smart->total_recv_read_claim_pages:%d;\n",
	.lc_184 = "[ND]smart->total_gc_times:%d;\n",
	.lc_185 = "[ND]smart->total_gc_pages:%d;\n",
	.lc_186 = "[ND]smart->total_wl_times:%d;\n",
	.lc_187 = "[ND]smart->total_wl_gc_times:%d;\n",
	.lc_188 = "[ND]smart->total_real_read_error_pages:%d;\n",
	.lc_189 = "[ND]smart->total_real_write_error_pages:%d;\n",
	.lc_190 = "[ND]smart->total_normal_power_cycles:%x;\n",
	.lc_191 = "[ND]smart->total_unusual_power_cycles:%x;\n",
	.lc_192 = "[ND]smart->max_block_erase_times:%d;\n",
	.lc_193 = "[ND]smart->min_block_erase_times:%d;\n",
	.lc_194 = "[ND]smart->max_block_used_counter:%d;\n",
	.lc_195 = "[ND]smart->min_block_used_counter:%d;\n",
	.lc_196 = "[ND]smart->read_reclaim_utc:%d;\n",
	.lc_197 = "[ND]smart->cross_talk_times:%d;\n",
	.lc_198 = "[ND]smart->read_reclaim_zone_no:%d;\n",
	.lc_199 = "[ND]smart->read_reclaim_page_no:%d;\n",
	.nand_crc16_table = {
		0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
		0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
		0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
		0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
		0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
		0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
		0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
		0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
		0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
		0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
		0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
		0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
		0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
		0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
		0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
		0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
		0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
		0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
		0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
		0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
		0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
		0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
		0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
		0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
		0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
		0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
		0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
		0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
		0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
		0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
		0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
		0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
	},
	.lc_146 = "[NE]error nand_write_logic_page_no_gc page_no:%u\n",
	.lc_147 = "[ND]logic_page_map_temp NULL!!\n",
	.lc_148 = "[NE]read a page: %d READ_RECLAIM!\n",
	.lc_149 = "[NE]no free block\n",
	.lc_150 = "[NE]do_write_error erase error sb:%d!\n",
	.lc_151 = "[NE]do_write_error sb@%d nftl_nand_copy_page error!\n",
	.lc_152 = "[NE]do after write error1!\n",
	.lc_153 = "[NE]do_before_write error!\n",
	.lc_154 = "[NE]write data to page error!\n",
	.lc_155 = "[NE]do after write error2!\n",
	.lc_156 = "[NE]get_new_current_block error3!\n",
	.lc_157 = "[NE]get_new_current_block error4!\n",
	.lc_158 = "[NE]write_phy_page_map_to_current_block error!\n",
	.lc_159 = "[NE]get_new_current_block error1!\n",
	.lc_160 = "[NE]get_new_current_block error2!\n",
	.lc_161 = "[NE]write logic data to current block error\n",
	.lc_162 = "[ND]last block can not used to logic page %d!!\n",
	.lc_163 = "[ND]last block can not used to power down page %d!!\n",
	.lc_164 = "[ND]write_phy_data_to_current_block write error %d!\n",
	.lc_165 = "[NE]do_write_error error\n",
	.lc_166 = "[NE]write_phy_data_to_current_block error1\n",
	.lc_167 = "[ND]logic_page_map NULL!!\n",
	.lc_168 = "[ND]invalid_page_count more than pages_per_blk at write logic data to current block:%d!!\n",
	.lc_169 = "[NE]write_phy_data_to_current_block error2\n",
	.lc_170 = "[NE]invalid_page_count more than pages_per_blk at write logic data to current block:%d!!\n",
	.lc_171 = "[NE]write phy data to current block error2\n",
	.lc_172 = "[ND]swl: new_cur_blk->alloc ok(max_ec): block_no = %d; ec = %d; \n",
	.lc_173 = "[NE]no free block!!!!\n",
	.lc_174 = "[NE]new_block_init_for_write erase error!\n",
	.lc_175 = "[NE]invaild_page num error:[%d] [%d]  used_page:[%d]!!\n",
	.lc_413 = "[NE]%s:malloc fail for cache_node!\n",
	.lc_414 = "[NE]%s:malloc fail for cache_page_buf!\n",
	.lc_415 = "[ND]read sector:0x%x,len:0x%x\n",
	.lc_416 = "[ND]write sector:0x%x,len:0x%x\n",
	.lc_417 = "[ND]dicard sector:0x%x,len:0x%x\n",
	.lc_418 = "[NE]paramter error. beyond reality capacity\n",
	.lc_419 = "[NI]nand_cache_read error buf is  NULL!\n",
	.lc_420 = "[NE]error1 node %d,%d,%d!\n",
	.lc_421 = "[NE]error1 bitmap %x!\n",
	.lc_422 = "[NE]ECC_LIMIT happened 12! page:%d!\n",
	.lc_423 = "[NE]error1 flush write cache to nand\n",
	.lc_424 = "[NE]error2 flush write cache to nand\n",
	.lc_425 = "[NI]ECC_LIMIT happened 11! page:%d!\n",
	.lc_426 = "[NE]flush write cache to nand no cache!\n",
	.lc_427 = "[NE]ECC_LIMIT happened 10! page:%d cache: %d!\n",
	.lc_428 = "[NE]flush write cache to nand error!\n",
	.lc_429 = "[ND]flush cache %d %d!\n",
	.lc_430 = "[NE]nand_discard paramter error!\n",
	.lc_431 = "[NI]page_map NULL!!\n",
	.lc_432 = "[ND]%4d:<%4d %3d><%d>\n",
	.lc_474 = "[NE]%s: malloc fail for zone\n",
	.lc_475 = "[NE]%s: malloc fail for nand_chip\n",
	.lc_476 = "[NE]%s: malloc fail for nand_block_info\n",
	.lc_477 = "[NE]not enough blocks %d!\n",
	.lc_478 = "[ND]nftl start:%d,%d\n",
	.lc_479 = "[NE]zone_param_init fail!\n",
	.lc_480 = "[NE]build_zone_list_first fail!\n",
	.lc_481 = "[NE]build_zone_list fail!\n",
	.lc_482 = "[NI]try again\n",
	.lc_433 = "[NE]garbage_collect phy_block null\n",
	.lc_434 = "[ND]garbage too frequency! %d:%d\n",
	.lc_435 = "[NE]garbage_collect_first wrong fbn:%d\n",
	.lc_436 = "[NI]garbage_collect_first none!!\n",
	.lc_437 = "[ND]garbage_collect_first wrong!!\n",
	.lc_438 = "[ND]garbage_collect_first cannot find PHY_MAPPING_PAGE%d %d %d %d!!\n",
	.lc_439 = "[ND]garbage_collect_first cannot find PHY_MAPPING_PAGE%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x.\n",
	.lc_440 = "[ND][NE]garbage_collect_first rebuild %d,%d!!\n",
	.lc_441 = "[ND]garbage_collect_first PHY_MAPPING_PAGEecc err block:%d !!\n",
	.lc_442 = "[NE]garbage_collect_first invaild_page num error!!\n",
	.lc_443 = "[ND]garbage_collect_first read a logic page error%d %d %d %d spare data:\n",
	.lc_444 = "%x %x %x %x %x %x %x %x %x %x %x %x %x %x %x %x\n",
	.lc_445 = "[NE]nand_write_logic_page_no_gc gc fail!!\n",
	.lc_446 = "[NE]erase ecc error block\n",
	.lc_447 = "[ND]no block to prio gc\n",
	.lc_448 = "[ND]no need gc:%d, type:%d\n",
	.lc_449 = "[NE]do_prio_gc garbage_collect_first fail:%d, type:%d\n",
	.lc_450 = "[ND]prio gc fail1:%d, type:%d\n",
	.lc_451 = "[ND]prio gc fail2:%d, type:%d\n",
	.lc_452 = "[ND]swl: static WL need to do(%d : %d)\n",
	.lc_453 = "[ND]swl: min_ec_blk_no = %d,\n",
	.lc_454 = "[ND]swl: zone->blocks = %d)\n",
	.lc_455 = "[ND]swl: the block(%d) for static WL is not in invalid_list\n",
	.lc_456 = "[ND]swl: done_static_wear_leveling(min_ec) block:%d, erase_count:%d\n",
	.lc_457 = "[NE]swl: garbage_collect_first fail:%d\n",
	.lc_458 = "[NE]swl: last->check fail:%d\n",
	.lc_459 = "[ND]gc_one ok! from: %d [%d %d %d] @%s %d\n",
	.lc_460 = "[NE][NE]gc_one error!\n",
	.lc_461 = "[ND][NE] gc_win_block gc_one error \n",
	.lc_462 = "[ND]not power on gc [%d, %d, %d], [%d %d %d]!\n",
	.lc_463 = "[ND]emergency power on gc %d !\n",
	.lc_464 = "[ND]power on gc %d!\n",
	.lc_465 = "[ND]%s %p %d %d %d %\n",
	.lc_466 = "[NI]shutdown_on_gc end! [%d %d %d]\n",
	.lc_467 = "[ND]shutdown_on_gc one ok! [%d %d %d]\n",
	.lc_468 = "%d %d %d %d %d %d/%d %d/%d\n",
	.lc_469 = "%08d:%04d ",
	.lc_18 = "[ND] panic part %s off %u size %u\n",
	.lc_19 = "[ND] reserved panic block before %d!\n",
	.lc_20 = "[NE] reserved panic block %d!\n",
	.lc_21 = "[NE] no reserved panic block!!! quit!\n",
	.lc_22 = "[ND] write panic data : logic addr: %d, sectors %d, pages %d, blocks %d\n",
	.lc_23 = "[NE] too much data for panic-write, %d sectors left over\n",
	.lc_24 = "[ND] write panic data block: %d page %d !\n",
	.lc_25 = "[ND] write dummy data block: %d page %d !\n",
	.lc_26 = "[ND] find panic block : %d\n",
	.lc_27 = "[NE] invalid panic block index %d\n",
	.lc_28 = "[ND] not find panic data, no need to recovery!\n",
	.lc_29 = "[ND] recovery panic data start!\n",
	.lc_30 = "[NE] %s(%d): malloc failed\n",
	.lc_31 = "[ND] ignore blk %d page %d as dummy page\n",
	.lc_32 = "[ND] exit recovery panic block\n",
	.lc_33 = "[ND] invalid blk %d page %d, finish recovery\n",
	.lc_34 = "[ND] move blk %d page %d to logic addr %d\n",
	.lc_35 = "rr value %x %x %x %x!\n",
	.lc_107 = "get_hynix_special_info start!!\n",
	.lc_108 = "hynix_special_info:can't find uboot head\n",
	.lc_109 = "physic info start_block %d\n",
	.lc_110 = "m1 retry!\n",
	.lc_111 = "m1 ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_104 = "fatal err -0, wrong input parameter, ch: %d  chip: %d/%d  block: %d/%d \n",
	.lc_112 = "m1 write page FF wrong\n",
	.lc_113 = "_major_check_byte, wrong input para, level %d, len %d\n",
	.lc_114 = "%d:  '1'-'0' : %d - %d\n",
	.lc_115 = "get bit %d failed!\n",
	.lc_116 = "_get_read_retry_parameters, get rr count failed!\n",
	.lc_117 = "rr cnt: %d\n",
	.lc_118 = "_get_read_retry_parameters, get rr reg count failed!\n",
	.lc_119 = "rr reg cnt: %d\n",
	.lc_120 = "read retry value from otp error: rr_cnt %d rr_reg_cnt %d!\n",
	.lc_121 = "_get_read_retry_parameters, get the %d-th byte of rr table failed!\n",
	.lc_123 = "start get read retry param from: ce %d, rb %d...\n",
	.lc_124 = "invalid buffer for otp info!\n",
	.lc_51 = "hynix OTP RegCount value error: 0x%x, 0x%x \n",
	.lc_125 = "otp_info_hynix_16nm : allocate memory fail\n",
	.lc_126 = "m1 CH %d chip %d get otp info failed!\n",
	.lc_127 = "m1 CH %d chip %d get read retry cfg from otp info failed!\n",
	.lc_54 = "_vender_get_param_otp_hynix time %d!\n",
	.lc_56 = "set retry default value: ",
	.lc_57 = " %x",
	.lc_59 = "[PHY_GetDefaultParam]:readbuf malloc fail\n",
	.lc_128 = "ch %d, chip %d Read Retry value Table from uboot\n",
	.lc_61 = "0x%x ",
	.lc_62 = "[PHY_DBG] ch %d, can't get right otp value from nand otp blocks, then use otp command\n",
	.lc_129 = "[PHY_DBG] repair ch %d chip %d otp value end\n",
	.lc_64 = "NFC_ReadRetryInit, unknown read retry mode 0x%x\n",
	.lc_130 = "hynix16nm_readretry_exit. \n",
	.lc_131 = " hynix16nm_special_init hynix16nm_read_retry_mode: %d hynix16nm_read_retry_cycle:%d hynix16nm_read_retry_reg_num:%d\n",
	.lc_132 = " hynix16nm_special_init error hynix16nm_read_retry_mode: %d hynix16nm_read_retry_cycle:%d hynix16nm_read_retry_reg_num:%d\n",
	.lc_133 = " hynix16nm_special_exit \n",
	.lc_36 = "samsung set readretry error ! %x \n",
	.lc_37 = "samsung retry!\n",
	.lc_38 = "samsung ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_39 = "samsung read retry conut: %d !\n",
	.lc_40 = " samsung_special_init samsung_read_retry_mode:%d samsung_read_retry_cycle :%d \n",
	.lc_41 = " samsung_special_init error samsung_read_retry_mode:%d samsung_read_retry_cycle :%d \n",
	.lc_42 = " samsung_special_exit \n",
	.para0 = {
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x000a, 0x0006,
		0x0000, 0xfffd, 0xfff9, 0xfff8, 0x0000, 0xfffa, 0xfff3, 0xfff1,
		0x0000, 0xfff5, 0xffec, 0xffe9, 0x0000, 0x0000, 0xffe6, 0xffe2,
		0x0000, 0x0000, 0xffe0, 0xffdb,
	},
	.para1 = {
		0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0006, 0x000a, 0x0006,
		0x0000, 0xfffd, 0xfff9, 0xfff8, 0x0000, 0xfffa, 0xfff3, 0xfff1,
		0x0000, 0xfff7, 0xffec, 0xffe9, 0x0000, 0x0000, 0xffe6, 0xffe2,
		0x0000, 0x0000, 0xffe0, 0xffdb,
	},
	.lc_85 = "hynix26nm retry param:",
	.lc_44 = " %x ",
	.lc_86 = "m0 retry!\n",
	.lc_87 = "hynix26nm ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_88 = "hynix26nm_lsb_exit\n",
	.lc_89 = "hynix26nm_get_default_param: ch: %d, chip: %d, value: 0x%x 0x%x 0x%x 0x%x \n",
	.lc_90 = "hynix26nm_readretry_exit. \n",
	.lc_91 = " hynix26nm_special_init hynix26nm_read_retry_mode: %d hynix26nm_read_retry_cycle:%d hynix26nm_read_retry_reg_num:%d\n",
	.lc_92 = " hynix26nm_special_init error hynix26nm_read_retry_mode: %d hynix26nm_read_retry_cycle:%d hynix26nm_read_retry_reg_num:%d\n",
	.lc_93 = " hynix26nm_special_exit \n",
	.lc_94 = "sandisk_A19 retry!\n",
	.lc_95 = "sandisk_A19 set readretry error\n",
	.lc_47 = "retry spare all 0xff! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_96 = "sandisk_A19 ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_97 = "sandisk_A19 ReadRetry ok with ecc limit! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_98 = "sandisk_A19 retry mode err : %d !\n",
	.lc_99 = "sandisk A19 read retry conut: %d !\n",
	.lc_100 = " sandisk_A19_special_init sandisk_A19_read_retry_mode:%d sandisk_A19_read_retry_cycle :%d sandisk_A19_read_retry_reg_cnt %d \n",
	.lc_101 = " sandisk_A19_special_init error sandisk_A19_read_retry_mode:%d sandisk_A19_read_retry_cycle :%d sandisk_A19_read_retry_reg_cnt %d \n",
	.lc_102 = "%s sandisk toggle vendor specific setting failed!\n",
	.lc_103 = " sandisk_A19_special_exit \n",
	.lc_105 = " generic special init \n",
	.lc_106 = " generic special exit \n",
	.lc_77 = "toshiba rr value 3 %x %x %x %x!\n",
	.lc_78 = "toshiba vender_pre_condition error!\n",
	.lc_79 = "toshiba retry!\n",
	.lc_80 = "toshiba ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_81 = "toshiba read retry conut: %d !\n",
	.lc_82 = " toshiba_special_init toshiba_read_retry_mode :%d toshiba_read_retry_cycle :%d toshiba_read_retry_reg_cnt :%d \n",
	.lc_83 = " toshiba_special_init error toshiba_read_retry_mode :%d toshiba_read_retry_cycle :%d toshiba_read_retry_reg_cnt :%d \n",
	.lc_84 = " toshiba_special_exit \n",
	.lc_69 = "set feature(addr %d) p0: %d readretry Configuration failed!\n",
	.lc_70 = "micron retry!\n",
	.lc_71 = "micron ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_72 = "micron read retry cannot support!\n",
	.lc_73 = "micron read retry conut: %d !\n",
	.lc_74 = " micron_special_init micron_read_retry_mode:%d micron_read_retry_cycle:%d \n",
	.lc_75 = " micron_special_init error micron_read_retry_mode:%d micron_read_retry_cycle:%d \n",
	.lc_76 = " micron_special_exit \n",
	.lc_134 = "sandisk retry!\n",
	.lc_135 = "sandisk ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_136 = "sandisk activation seq fail 0x%x\n",
	.lc_1 = "SSSSSSSSS",
	.lc_137 = "sandisk read retry conut: %d !\n",
	.lc_138 = " sandisk_special_init sandisk_read_retry_mode:%d sandisk_read_retry_cycle :%d sandisk_read_retry_reg_cnt %d \n",
	.lc_139 = " sandisk_special_init error sandisk_read_retry_mode:%d sandisk_read_retry_cycle :%d sandisk_read_retry_reg_cnt %d \n",
	.lc_140 = " sandisk_special_exit \n",
	.lc_43 = "retry para:",
	.lc_46 = "hynix20nm retry!\n",
	.lc_48 = "sdata:0x%x %x %x %x ",
	.lc_49 = "hynix20nm ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
	.lc_50 = "hynix20nm_lsb_exit\n",
	.lc_52 = "otp copy %d is ok!\n",
	.lc_53 = "otp copy failed!\n",
	.lc_55 = "_vender_get_param_otp_hynix error!\n",
	.lc_58 = "hynix20nm get default param error !\n",
	.lc_60 = "ch %d, chip %d Read Retry value Table from uboot:\n",
	.lc_63 = "[PHY_DBG] repair ch %d otp value end\n",
	.lc_65 = "hynix20nm_readretry_exit. \n",
	.lc_66 = " hynix20nm_special_init hynix20nm_read_retry_mode: %d hynix20nm_read_retry_cycle:%d hynix20nm_read_retry_reg_num:%d\n",
	.lc_67 = " hynix20nm_special_init error hynix20nm_read_retry_mode: %d hynix20nm_read_retry_cycle:%d hynix20nm_read_retry_reg_num:%d\n",
	.lc_68 = " hynix20nm_special_exit \n",
	.func_5346 = "nftl_initialize",
	.func_5194 = "nftl_add",
	.func_7098 = "zone_param_init",
	.func_7312 = "init_nrc",
	.func_7402 = "torture_block_and_mark",
	.func_7526 = "block_last_page_ecc_error",
	.func_7558 = "nand_dbg_phy_read",
	.func_7569 = "nand_dbg_zone_phy_read",
	.func_7580 = "nand_dbg_zone_phy_write",
	.func_7589 = "nand_dbg_phy_write",
	.func_7642 = "write_data_to_nand",
	.function_7666 = "nand_power_off_ecc_error",
	.func_6755 = "nftl_sector_cache_init",
	.func_8479 = "get_logic_page_map",
	.func_7884 = "nftl_cache_init",
	.func_8536 = "nftl_start",
	.func_8331 = "shutdown_on_gc",
	.func_5524 = "panic_recover_data",
	.func_6409 = "sandisk_A19_special_exit",
};

_Static_assert(sizeof(struct nftl_readonly_layout) == 22105,
	"NFTL readonly span changed");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, nand_crc16_table) == 10736,
	"NFTL CRC16 table offset changed");

__asm__(".global com\n"
        ".set com, .Lnftl_readonly_storage+0\n"
        ".type com, %object\n"
        ".size com, 11");

__asm__(".global mod\n"
        ".set mod, .Lnftl_readonly_storage+16\n"
        ".type mod, %object\n"
        ".size mod, 6");

__asm__(".global para0\n"
        ".set para0, .Lnftl_readonly_storage+18136\n"
        ".type para0, %object\n"
        ".size para0, 56");

__asm__(".global para1\n"
        ".set para1, .Lnftl_readonly_storage+18192\n"
        ".type para1, %object\n"
        ".size para1, 56");

__asm__(""
        ".set __func__.5346, .Lnftl_readonly_storage+21688\n"
        ".type __func__.5346, %object\n"
        ".size __func__.5346, 16");

__asm__(""
        ".set __func__.5194, .Lnftl_readonly_storage+21704\n"
        ".type __func__.5194, %object\n"
        ".size __func__.5194, 9");

__asm__(""
        ".set __func__.7098, .Lnftl_readonly_storage+21720\n"
        ".type __func__.7098, %object\n"
        ".size __func__.7098, 16");

__asm__(""
        ".set __func__.7312, .Lnftl_readonly_storage+21736\n"
        ".type __func__.7312, %object\n"
        ".size __func__.7312, 9");

__asm__(""
        ".set __func__.7402, .Lnftl_readonly_storage+21752\n"
        ".type __func__.7402, %object\n"
        ".size __func__.7402, 23");

__asm__(""
        ".set __func__.7526, .Lnftl_readonly_storage+21776\n"
        ".type __func__.7526, %object\n"
        ".size __func__.7526, 26");

__asm__(""
        ".set __func__.7558, .Lnftl_readonly_storage+21808\n"
        ".type __func__.7558, %object\n"
        ".size __func__.7558, 18");

__asm__(""
        ".set __func__.7569, .Lnftl_readonly_storage+21832\n"
        ".type __func__.7569, %object\n"
        ".size __func__.7569, 23");

__asm__(""
        ".set __func__.7580, .Lnftl_readonly_storage+21856\n"
        ".type __func__.7580, %object\n"
        ".size __func__.7580, 24");

__asm__(""
        ".set __func__.7589, .Lnftl_readonly_storage+21880\n"
        ".type __func__.7589, %object\n"
        ".size __func__.7589, 19");

__asm__(""
        ".set __func__.7642, .Lnftl_readonly_storage+21904\n"
        ".type __func__.7642, %object\n"
        ".size __func__.7642, 19");

__asm__(""
        ".set __FUNCTION__.7666, .Lnftl_readonly_storage+21928\n"
        ".type __FUNCTION__.7666, %object\n"
        ".size __FUNCTION__.7666, 25");

__asm__(""
        ".set __func__.6755, .Lnftl_readonly_storage+21960\n"
        ".type __func__.6755, %object\n"
        ".size __func__.6755, 23");

__asm__(""
        ".set __func__.8479, .Lnftl_readonly_storage+21984\n"
        ".type __func__.8479, %object\n"
        ".size __func__.8479, 19");

__asm__(""
        ".set __func__.7884, .Lnftl_readonly_storage+22008\n"
        ".type __func__.7884, %object\n"
        ".size __func__.7884, 16");

__asm__(""
        ".set __func__.8536, .Lnftl_readonly_storage+22024\n"
        ".type __func__.8536, %object\n"
        ".size __func__.8536, 11");

__asm__(""
        ".set __func__.8331, .Lnftl_readonly_storage+22040\n"
        ".type __func__.8331, %object\n"
        ".size __func__.8331, 15");

__asm__(""
        ".set __func__.5524, .Lnftl_readonly_storage+22056\n"
        ".type __func__.5524, %object\n"
        ".size __func__.5524, 19");

__asm__(""
        ".set __func__.6409, .Lnftl_readonly_storage+22080\n"
        ".type __func__.6409, %object\n"
        ".size __func__.6409, 25");

__asm__(".global nand_crc16_table\n"
        ".set nand_crc16_table, .Lnftl_readonly_storage+10736\n"
        ".type nand_crc16_table, %object\n"
        ".size nand_crc16_table, 512");

/* C helper views retain the same table addresses without storage. */
#define NFTL_RO_VIEW(alias, offset) \
	__asm__(".global " #alias "\n" \
		".set " #alias ", .Lnftl_readonly_storage+" #offset "\n" \
		".size " #alias ", 0")
NFTL_RO_VIEW(hynix26nm_para0_recovered, 18136);
NFTL_RO_VIEW(hynix26nm_para1_recovered, 18192);
/* Five Sandisk retry diagnostics used by the maintainable C implementation.
 * Their offsets are rooted in the stock Image's ADRP/ADD call sites. */
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_94) == 18800,
	       "Sandisk retry diagnostic offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_95) == 18824,
	       "Sandisk set-error diagnostic offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_47) == 18864,
	       "Sandisk spare diagnostic offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_96) == 18952,
	       "Sandisk success diagnostic offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_97) == 19040,
	       "Sandisk ECC-limit diagnostic offset");
NFTL_RO_VIEW(nftl_ro_lc_94, 18800);
NFTL_RO_VIEW(nftl_ro_lc_95, 18824);
NFTL_RO_VIEW(nftl_ro_lc_47, 18864);
NFTL_RO_VIEW(nftl_ro_lc_96, 18952);
NFTL_RO_VIEW(nftl_ro_lc_97, 19040);

/* C diagnostics reuse the stock pool. Compile-time offsets guard every view. */
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_9) == 24, "NFTL lc_9 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_10) == 56, "NFTL lc_10 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_11) == 64, "NFTL lc_11 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_12) == 80, "NFTL lc_12 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_13) == 88, "NFTL lc_13 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_14) == 96, "NFTL lc_14 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_15) == 104, "NFTL lc_15 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_16) == 144, "NFTL lc_16 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_17) == 184, "NFTL lc_17 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_3) == 232, "NFTL lc_3 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_4) == 264, "NFTL lc_4 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_5) == 304, "NFTL lc_5 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_6) == 336, "NFTL lc_6 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_7) == 360, "NFTL lc_7 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_8) == 384, "NFTL lc_8 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_400) == 416, "NFTL lc_400 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_401) == 440, "NFTL lc_401 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_402) == 472, "NFTL lc_402 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_403) == 488, "NFTL lc_403 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_404) == 504, "NFTL lc_404 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_405) == 536, "NFTL lc_405 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_406) == 568, "NFTL lc_406 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_407) == 600, "NFTL lc_407 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_408) == 624, "NFTL lc_408 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_409) == 656, "NFTL lc_409 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_410) == 680, "NFTL lc_410 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_411) == 712, "NFTL lc_411 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_412) == 776, "NFTL lc_412 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_45) == 832, "NFTL lc_45 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_200) == 840, "NFTL lc_200 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_122) == 848, "NFTL lc_122 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_201) == 856, "NFTL lc_201 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_202) == 904, "NFTL lc_202 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_203) == 944, "NFTL lc_203 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_204) == 984, "NFTL lc_204 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_205) == 1016, "NFTL lc_205 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_206) == 1072, "NFTL lc_206 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_207) == 1128, "NFTL lc_207 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_208) == 1184, "NFTL lc_208 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_209) == 1208, "NFTL lc_209 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_210) == 1240, "NFTL lc_210 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_211) == 1264, "NFTL lc_211 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_212) == 1280, "NFTL lc_212 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_213) == 1320, "NFTL lc_213 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_214) == 1360, "NFTL lc_214 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_215) == 1376, "NFTL lc_215 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_216) == 1392, "NFTL lc_216 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_217) == 1416, "NFTL lc_217 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_218) == 1440, "NFTL lc_218 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_219) == 1472, "NFTL lc_219 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_220) == 1496, "NFTL lc_220 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_221) == 1584, "NFTL lc_221 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_222) == 1608, "NFTL lc_222 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_223) == 1640, "NFTL lc_223 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_224) == 1672, "NFTL lc_224 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_225) == 1696, "NFTL lc_225 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_226) == 1728, "NFTL lc_226 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_227) == 1760, "NFTL lc_227 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_228) == 1808, "NFTL lc_228 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_229) == 1888, "NFTL lc_229 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_230) == 1920, "NFTL lc_230 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_231) == 1952, "NFTL lc_231 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_232) == 2040, "NFTL lc_232 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_233) == 2088, "NFTL lc_233 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_234) == 2144, "NFTL lc_234 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_235) == 2200, "NFTL lc_235 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_236) == 2256, "NFTL lc_236 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_237) == 2312, "NFTL lc_237 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_238) == 2368, "NFTL lc_238 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_239) == 2456, "NFTL lc_239 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_240) == 2504, "NFTL lc_240 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_241) == 2544, "NFTL lc_241 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_242) == 2576, "NFTL lc_242 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_243) == 2600, "NFTL lc_243 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_244) == 2640, "NFTL lc_244 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_245) == 2720, "NFTL lc_245 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_246) == 2800, "NFTL lc_246 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_247) == 2832, "NFTL lc_247 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_248) == 2872, "NFTL lc_248 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_249) == 2912, "NFTL lc_249 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_250) == 2984, "NFTL lc_250 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_251) == 3024, "NFTL lc_251 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_252) == 3056, "NFTL lc_252 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_253) == 3080, "NFTL lc_253 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_254) == 3128, "NFTL lc_254 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_255) == 3176, "NFTL lc_255 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_256) == 3216, "NFTL lc_256 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_257) == 3256, "NFTL lc_257 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_258) == 3344, "NFTL lc_258 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_259) == 3432, "NFTL lc_259 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_260) == 3472, "NFTL lc_260 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_261) == 3504, "NFTL lc_261 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_262) == 3536, "NFTL lc_262 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_263) == 3592, "NFTL lc_263 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_264) == 3632, "NFTL lc_264 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_265) == 3696, "NFTL lc_265 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_266) == 3728, "NFTL lc_266 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_267) == 3760, "NFTL lc_267 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_268) == 3816, "NFTL lc_268 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_269) == 3880, "NFTL lc_269 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_270) == 3936, "NFTL lc_270 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_271) == 3992, "NFTL lc_271 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_272) == 4024, "NFTL lc_272 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_273) == 4080, "NFTL lc_273 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_274) == 4112, "NFTL lc_274 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_275) == 4168, "NFTL lc_275 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_276) == 4232, "NFTL lc_276 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_277) == 4288, "NFTL lc_277 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_278) == 4336, "NFTL lc_278 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_279) == 4384, "NFTL lc_279 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_280) == 4400, "NFTL lc_280 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_281) == 4416, "NFTL lc_281 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_282) == 4432, "NFTL lc_282 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_283) == 4448, "NFTL lc_283 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_284) == 4488, "NFTL lc_284 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_285) == 4536, "NFTL lc_285 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_286) == 4592, "NFTL lc_286 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_287) == 4640, "NFTL lc_287 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_288) == 4704, "NFTL lc_288 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_289) == 4752, "NFTL lc_289 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_290) == 4792, "NFTL lc_290 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_291) == 4832, "NFTL lc_291 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_292) == 4864, "NFTL lc_292 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_293) == 4904, "NFTL lc_293 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_294) == 4928, "NFTL lc_294 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_295) == 4952, "NFTL lc_295 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_296) == 5000, "NFTL lc_296 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_297) == 5032, "NFTL lc_297 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_298) == 5080, "NFTL lc_298 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_299) == 5144, "NFTL lc_299 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_300) == 5176, "NFTL lc_300 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_301) == 5208, "NFTL lc_301 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_302) == 5232, "NFTL lc_302 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_303) == 5264, "NFTL lc_303 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_304) == 5296, "NFTL lc_304 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_305) == 5352, "NFTL lc_305 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_306) == 5408, "NFTL lc_306 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_307) == 5448, "NFTL lc_307 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_308) == 5504, "NFTL lc_308 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_309) == 5536, "NFTL lc_309 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_310) == 5640, "NFTL lc_310 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_311) == 5672, "NFTL lc_311 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_312) == 5704, "NFTL lc_312 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_313) == 5768, "NFTL lc_313 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_314) == 5816, "NFTL lc_314 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_315) == 5856, "NFTL lc_315 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_316) == 5888, "NFTL lc_316 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_317) == 5928, "NFTL lc_317 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_318) == 5968, "NFTL lc_318 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_319) == 6024, "NFTL lc_319 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_320) == 6080, "NFTL lc_320 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_321) == 6128, "NFTL lc_321 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_322) == 6168, "NFTL lc_322 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_323) == 6192, "NFTL lc_323 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_324) == 6216, "NFTL lc_324 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_325) == 6272, "NFTL lc_325 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_326) == 6320, "NFTL lc_326 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_327) == 6360, "NFTL lc_327 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_328) == 6392, "NFTL lc_328 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_329) == 6424, "NFTL lc_329 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_330) == 6440, "NFTL lc_330 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_331) == 6480, "NFTL lc_331 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_332) == 6528, "NFTL lc_332 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_333) == 6576, "NFTL lc_333 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_334) == 6624, "NFTL lc_334 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_335) == 6672, "NFTL lc_335 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_336) == 6696, "NFTL lc_336 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_337) == 6720, "NFTL lc_337 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_338) == 6752, "NFTL lc_338 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_339) == 6776, "NFTL lc_339 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_340) == 6808, "NFTL lc_340 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_341) == 6848, "NFTL lc_341 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_342) == 6888, "NFTL lc_342 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_343) == 6920, "NFTL lc_343 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_344) == 6976, "NFTL lc_344 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_345) == 7024, "NFTL lc_345 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_346) == 7064, "NFTL lc_346 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_347) == 7104, "NFTL lc_347 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_348) == 7144, "NFTL lc_348 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_349) == 7208, "NFTL lc_349 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_350) == 7264, "NFTL lc_350 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_351) == 7312, "NFTL lc_351 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_352) == 7352, "NFTL lc_352 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_353) == 7400, "NFTL lc_353 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_354) == 7464, "NFTL lc_354 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_355) == 7504, "NFTL lc_355 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_356) == 7568, "NFTL lc_356 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_357) == 7608, "NFTL lc_357 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_358) == 7648, "NFTL lc_358 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_359) == 7688, "NFTL lc_359 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_360) == 7752, "NFTL lc_360 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_361) == 7792, "NFTL lc_361 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_362) == 7856, "NFTL lc_362 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_363) == 7896, "NFTL lc_363 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_364) == 7928, "NFTL lc_364 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_365) == 7960, "NFTL lc_365 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_366) == 8000, "NFTL lc_366 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_367) == 8032, "NFTL lc_367 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_368) == 8064, "NFTL lc_368 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_369) == 8112, "NFTL lc_369 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_370) == 8160, "NFTL lc_370 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_371) == 8216, "NFTL lc_371 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_372) == 8264, "NFTL lc_372 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_373) == 8304, "NFTL lc_373 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_374) == 8352, "NFTL lc_374 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_375) == 8408, "NFTL lc_375 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_376) == 8456, "NFTL lc_376 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_377) == 8512, "NFTL lc_377 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_378) == 8576, "NFTL lc_378 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_379) == 8616, "NFTL lc_379 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_380) == 8664, "NFTL lc_380 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_381) == 8704, "NFTL lc_381 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_382) == 8752, "NFTL lc_382 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_383) == 8784, "NFTL lc_383 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_384) == 8848, "NFTL lc_384 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_385) == 8896, "NFTL lc_385 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_386) == 8928, "NFTL lc_386 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_387) == 8960, "NFTL lc_387 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_388) == 8992, "NFTL lc_388 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_389) == 9024, "NFTL lc_389 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_390) == 9056, "NFTL lc_390 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_391) == 9136, "NFTL lc_391 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_392) == 9168, "NFTL lc_392 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_393) == 9200, "NFTL lc_393 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_394) == 9232, "NFTL lc_394 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_395) == 9240, "NFTL lc_395 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_396) == 9272, "NFTL lc_396 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_397) == 9296, "NFTL lc_397 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_398) == 9320, "NFTL lc_398 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_399) == 9344, "NFTL lc_399 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_141) == 9360, "NFTL lc_141 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_142) == 9400, "NFTL lc_142 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_143) == 9432, "NFTL lc_143 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_144) == 9464, "NFTL lc_144 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_145) == 9496, "NFTL lc_145 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_470) == 9536, "NFTL lc_470 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_471) == 9584, "NFTL lc_471 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_472) == 9632, "NFTL lc_472 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_473) == 9680, "NFTL lc_473 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_176) == 9736, "NFTL lc_176 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_177) == 9760, "NFTL lc_177 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_178) == 9808, "NFTL lc_178 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_179) == 9856, "NFTL lc_179 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_180) == 9896, "NFTL lc_180 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_181) == 9944, "NFTL lc_181 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_182) == 9992, "NFTL lc_182 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_183) == 10040, "NFTL lc_183 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_184) == 10088, "NFTL lc_184 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_185) == 10120, "NFTL lc_185 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_186) == 10152, "NFTL lc_186 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_187) == 10184, "NFTL lc_187 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_188) == 10224, "NFTL lc_188 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_189) == 10272, "NFTL lc_189 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_190) == 10320, "NFTL lc_190 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_191) == 10368, "NFTL lc_191 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_192) == 10416, "NFTL lc_192 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_193) == 10456, "NFTL lc_193 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_194) == 10496, "NFTL lc_194 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_195) == 10536, "NFTL lc_195 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_196) == 10576, "NFTL lc_196 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_197) == 10616, "NFTL lc_197 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_198) == 10656, "NFTL lc_198 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_199) == 10696, "NFTL lc_199 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_146) == 11248, "NFTL lc_146 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_147) == 11304, "NFTL lc_147 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_148) == 11336, "NFTL lc_148 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_149) == 11376, "NFTL lc_149 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_150) == 11400, "NFTL lc_150 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_151) == 11440, "NFTL lc_151 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_152) == 11496, "NFTL lc_152 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_153) == 11528, "NFTL lc_153 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_154) == 11560, "NFTL lc_154 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_155) == 11592, "NFTL lc_155 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_156) == 11624, "NFTL lc_156 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_157) == 11664, "NFTL lc_157 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_158) == 11704, "NFTL lc_158 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_159) == 11752, "NFTL lc_159 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_160) == 11792, "NFTL lc_160 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_161) == 11832, "NFTL lc_161 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_162) == 11880, "NFTL lc_162 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_163) == 11928, "NFTL lc_163 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_164) == 11984, "NFTL lc_164 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_165) == 12040, "NFTL lc_165 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_166) == 12072, "NFTL lc_166 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_167) == 12120, "NFTL lc_167 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_168) == 12152, "NFTL lc_168 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_169) == 12248, "NFTL lc_169 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_170) == 12296, "NFTL lc_170 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_171) == 12392, "NFTL lc_171 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_172) == 12440, "NFTL lc_172 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_173) == 12512, "NFTL lc_173 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_174) == 12536, "NFTL lc_174 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_175) == 12584, "NFTL lc_175 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_413) == 12640, "NFTL lc_413 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_414) == 12680, "NFTL lc_414 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_415) == 12720, "NFTL lc_415 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_416) == 12752, "NFTL lc_416 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_417) == 12784, "NFTL lc_417 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_418) == 12824, "NFTL lc_418 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_419) == 12872, "NFTL lc_419 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_420) == 12920, "NFTL lc_420 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_421) == 12952, "NFTL lc_421 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_422) == 12976, "NFTL lc_422 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_423) == 13016, "NFTL lc_423 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_424) == 13056, "NFTL lc_424 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_425) == 13096, "NFTL lc_425 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_426) == 13136, "NFTL lc_426 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_427) == 13184, "NFTL lc_427 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_428) == 13232, "NFTL lc_428 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_429) == 13272, "NFTL lc_429 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_430) == 13296, "NFTL lc_430 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_431) == 13336, "NFTL lc_431 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_432) == 13360, "NFTL lc_432 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_474) == 13384, "NFTL lc_474 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_475) == 13416, "NFTL lc_475 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_476) == 13456, "NFTL lc_476 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_477) == 13504, "NFTL lc_477 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_478) == 13536, "NFTL lc_478 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_479) == 13560, "NFTL lc_479 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_480) == 13592, "NFTL lc_480 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_481) == 13632, "NFTL lc_481 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_482) == 13664, "NFTL lc_482 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_433) == 13680, "NFTL lc_433 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_434) == 13720, "NFTL lc_434 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_435) == 13760, "NFTL lc_435 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_436) == 13800, "NFTL lc_436 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_437) == 13840, "NFTL lc_437 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_438) == 13880, "NFTL lc_438 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_439) == 13952, "NFTL lc_439 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_440) == 14056, "NFTL lc_440 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_441) == 14104, "NFTL lc_441 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_442) == 14168, "NFTL lc_442 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_443) == 14224, "NFTL lc_443 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_444) == 14304, "NFTL lc_444 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_445) == 14360, "NFTL lc_445 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_446) == 14408, "NFTL lc_446 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_447) == 14440, "NFTL lc_447 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_448) == 14472, "NFTL lc_448 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_449) == 14504, "NFTL lc_449 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_450) == 14560, "NFTL lc_450 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_451) == 14592, "NFTL lc_451 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_452) == 14624, "NFTL lc_452 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_453) == 14664, "NFTL lc_453 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_454) == 14696, "NFTL lc_454 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_455) == 14728, "NFTL lc_455 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_456) == 14792, "NFTL lc_456 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_457) == 14864, "NFTL lc_457 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_458) == 14904, "NFTL lc_458 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_459) == 14936, "NFTL lc_459 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_460) == 14984, "NFTL lc_460 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_461) == 15008, "NFTL lc_461 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_462) == 15048, "NFTL lc_462 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_463) == 15096, "NFTL lc_463 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_464) == 15128, "NFTL lc_464 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_465) == 15152, "NFTL lc_465 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_466) == 15176, "NFTL lc_466 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_467) == 15216, "NFTL lc_467 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_468) == 15256, "NFTL lc_468 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_469) == 15288, "NFTL lc_469 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_18) == 15304, "NFTL lc_18 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_19) == 15344, "NFTL lc_19 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_20) == 15384, "NFTL lc_20 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_21) == 15416, "NFTL lc_21 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_22) == 15456, "NFTL lc_22 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_23) == 15536, "NFTL lc_23 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_24) == 15600, "NFTL lc_24 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_25) == 15648, "NFTL lc_25 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_26) == 15696, "NFTL lc_26 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_27) == 15728, "NFTL lc_27 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_28) == 15768, "NFTL lc_28 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_29) == 15816, "NFTL lc_29 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_30) == 15856, "NFTL lc_30 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_31) == 15888, "NFTL lc_31 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_32) == 15936, "NFTL lc_32 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_33) == 15968, "NFTL lc_33 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_34) == 16016, "NFTL lc_34 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_35) == 16064, "NFTL lc_35 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_107) == 16088, "NFTL lc_107 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_108) == 16120, "NFTL lc_108 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_109) == 16168, "NFTL lc_109 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_110) == 16200, "NFTL lc_110 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_111) == 16216, "NFTL lc_111 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_104) == 16296, "NFTL lc_104 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_112) == 16376, "NFTL lc_112 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_113) == 16400, "NFTL lc_113 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_114) == 16456, "NFTL lc_114 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_115) == 16480, "NFTL lc_115 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_116) == 16504, "NFTL lc_116 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_117) == 16560, "NFTL lc_117 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_118) == 16576, "NFTL lc_118 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_119) == 16632, "NFTL lc_119 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_120) == 16648, "NFTL lc_120 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_121) == 16712, "NFTL lc_121 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_123) == 16784, "NFTL lc_123 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_124) == 16840, "NFTL lc_124 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_51) == 16872, "NFTL lc_51 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_125) == 16920, "NFTL lc_125 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_126) == 16968, "NFTL lc_126 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_127) == 17008, "NFTL lc_127 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_54) == 17072, "NFTL lc_54 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_56) == 17112, "NFTL lc_56 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_57) == 17144, "NFTL lc_57 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_59) == 17152, "NFTL lc_59 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_128) == 17200, "NFTL lc_128 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_61) == 17256, "NFTL lc_61 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_62) == 17264, "NFTL lc_62 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_129) == 17352, "NFTL lc_129 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_64) == 17400, "NFTL lc_64 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_130) == 17456, "NFTL lc_130 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_131) == 17488, "NFTL lc_131 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_132) == 17608, "NFTL lc_132 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_36) == 17768, "NFTL lc_36 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_37) == 17808, "NFTL lc_37 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_38) == 17824, "NFTL lc_38 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_39) == 17912, "NFTL lc_39 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_40) == 17944, "NFTL lc_40 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_41) == 18024, "NFTL lc_41 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_85) == 18248, "NFTL lc_85 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_44) == 18272, "NFTL lc_44 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_86) == 18280, "NFTL lc_86 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_87) == 18296, "NFTL lc_87 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_89) == 18408, "NFTL lc_89 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_90) == 18488, "NFTL lc_90 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_91) == 18520, "NFTL lc_91 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_92) == 18640, "NFTL lc_92 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_98) == 19144, "NFTL lc_98 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_99) == 19184, "NFTL lc_99 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_100) == 19224, "NFTL lc_100 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_101) == 19352, "NFTL lc_101 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_102) == 19488, "NFTL lc_102 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_103) == 19544, "NFTL lc_103 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_105) == 19576, "NFTL lc_105 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_106) == 19600, "NFTL lc_106 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_77) == 19624, "NFTL lc_77 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_78) == 19664, "NFTL lc_78 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_79) == 19704, "NFTL lc_79 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_80) == 19720, "NFTL lc_80 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_81) == 19808, "NFTL lc_81 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_82) == 19840, "NFTL lc_82 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_83) == 19952, "NFTL lc_83 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_69) == 20096, "NFTL lc_69 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_70) == 20160, "NFTL lc_70 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_71) == 20176, "NFTL lc_71 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_72) == 20264, "NFTL lc_72 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_73) == 20304, "NFTL lc_73 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_74) == 20336, "NFTL lc_74 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_75) == 20416, "NFTL lc_75 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_134) == 20528, "NFTL lc_134 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_135) == 20544, "NFTL lc_135 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_136) == 20632, "NFTL lc_136 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_137) == 20688, "NFTL lc_137 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_138) == 20720, "NFTL lc_138 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_139) == 20832, "NFTL lc_139 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_43) == 20976, "NFTL lc_43 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_46) == 20992, "NFTL lc_46 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_48) == 21016, "NFTL lc_48 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_49) == 21040, "NFTL lc_49 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_52) == 21152, "NFTL lc_52 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_53) == 21176, "NFTL lc_53 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_55) == 21200, "NFTL lc_55 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_58) == 21240, "NFTL lc_58 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_60) == 21280, "NFTL lc_60 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_63) == 21336, "NFTL lc_63 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_65) == 21376, "NFTL lc_65 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_66) == 21408, "NFTL lc_66 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, lc_67) == 21528, "NFTL lc_67 offset");
NFTL_RO_VIEW(nftl_ro_lc_9, 24);
NFTL_RO_VIEW(nftl_ro_lc_10, 56);
NFTL_RO_VIEW(nftl_ro_lc_11, 64);
NFTL_RO_VIEW(nftl_ro_lc_12, 80);
NFTL_RO_VIEW(nftl_ro_lc_13, 88);
NFTL_RO_VIEW(nftl_ro_lc_14, 96);
NFTL_RO_VIEW(nftl_ro_lc_15, 104);
NFTL_RO_VIEW(nftl_ro_lc_16, 144);
NFTL_RO_VIEW(nftl_ro_lc_17, 184);
NFTL_RO_VIEW(nftl_ro_lc_3, 232);
NFTL_RO_VIEW(nftl_ro_lc_4, 264);
NFTL_RO_VIEW(nftl_ro_lc_5, 304);
NFTL_RO_VIEW(nftl_ro_lc_6, 336);
NFTL_RO_VIEW(nftl_ro_lc_7, 360);
NFTL_RO_VIEW(nftl_ro_lc_8, 384);
NFTL_RO_VIEW(nftl_ro_lc_400, 416);
NFTL_RO_VIEW(nftl_ro_lc_401, 440);
NFTL_RO_VIEW(nftl_ro_lc_402, 472);
NFTL_RO_VIEW(nftl_ro_lc_403, 488);
NFTL_RO_VIEW(nftl_ro_lc_404, 504);
NFTL_RO_VIEW(nftl_ro_lc_405, 536);
NFTL_RO_VIEW(nftl_ro_lc_406, 568);
NFTL_RO_VIEW(nftl_ro_lc_407, 600);
NFTL_RO_VIEW(nftl_ro_lc_408, 624);
NFTL_RO_VIEW(nftl_ro_lc_409, 656);
NFTL_RO_VIEW(nftl_ro_lc_410, 680);
NFTL_RO_VIEW(nftl_ro_lc_411, 712);
NFTL_RO_VIEW(nftl_ro_lc_412, 776);
NFTL_RO_VIEW(nftl_ro_lc_45, 832);
NFTL_RO_VIEW(nftl_ro_lc_200, 840);
NFTL_RO_VIEW(nftl_ro_lc_122, 848);
NFTL_RO_VIEW(nftl_ro_lc_201, 856);
NFTL_RO_VIEW(nftl_ro_lc_202, 904);
NFTL_RO_VIEW(nftl_ro_lc_203, 944);
NFTL_RO_VIEW(nftl_ro_lc_204, 984);
NFTL_RO_VIEW(nftl_ro_lc_205, 1016);
NFTL_RO_VIEW(nftl_ro_lc_206, 1072);
NFTL_RO_VIEW(nftl_ro_lc_207, 1128);
NFTL_RO_VIEW(nftl_ro_lc_208, 1184);
NFTL_RO_VIEW(nftl_ro_lc_209, 1208);
NFTL_RO_VIEW(nftl_ro_lc_210, 1240);
NFTL_RO_VIEW(nftl_ro_lc_211, 1264);
NFTL_RO_VIEW(nftl_ro_lc_212, 1280);
NFTL_RO_VIEW(nftl_ro_lc_213, 1320);
NFTL_RO_VIEW(nftl_ro_lc_214, 1360);
NFTL_RO_VIEW(nftl_ro_lc_215, 1376);
NFTL_RO_VIEW(nftl_ro_lc_216, 1392);
NFTL_RO_VIEW(nftl_ro_lc_217, 1416);
NFTL_RO_VIEW(nftl_ro_lc_218, 1440);
NFTL_RO_VIEW(nftl_ro_lc_219, 1472);
NFTL_RO_VIEW(nftl_ro_lc_220, 1496);
NFTL_RO_VIEW(nftl_ro_lc_221, 1584);
NFTL_RO_VIEW(nftl_ro_lc_222, 1608);
NFTL_RO_VIEW(nftl_ro_lc_223, 1640);
NFTL_RO_VIEW(nftl_ro_lc_224, 1672);
NFTL_RO_VIEW(nftl_ro_lc_225, 1696);
NFTL_RO_VIEW(nftl_ro_lc_226, 1728);
NFTL_RO_VIEW(nftl_ro_lc_227, 1760);
NFTL_RO_VIEW(nftl_ro_lc_228, 1808);
NFTL_RO_VIEW(nftl_ro_lc_229, 1888);
NFTL_RO_VIEW(nftl_ro_lc_230, 1920);
NFTL_RO_VIEW(nftl_ro_lc_231, 1952);
NFTL_RO_VIEW(nftl_ro_lc_232, 2040);
NFTL_RO_VIEW(nftl_ro_lc_233, 2088);
NFTL_RO_VIEW(nftl_ro_lc_234, 2144);
NFTL_RO_VIEW(nftl_ro_lc_235, 2200);
NFTL_RO_VIEW(nftl_ro_lc_236, 2256);
NFTL_RO_VIEW(nftl_ro_lc_237, 2312);
NFTL_RO_VIEW(nftl_ro_lc_238, 2368);
NFTL_RO_VIEW(nftl_ro_lc_239, 2456);
NFTL_RO_VIEW(nftl_ro_lc_240, 2504);
NFTL_RO_VIEW(nftl_ro_lc_241, 2544);
NFTL_RO_VIEW(nftl_ro_lc_242, 2576);
NFTL_RO_VIEW(nftl_ro_lc_243, 2600);
NFTL_RO_VIEW(nftl_ro_lc_244, 2640);
NFTL_RO_VIEW(nftl_ro_lc_245, 2720);
NFTL_RO_VIEW(nftl_ro_lc_246, 2800);
NFTL_RO_VIEW(nftl_ro_lc_247, 2832);
NFTL_RO_VIEW(nftl_ro_lc_248, 2872);
NFTL_RO_VIEW(nftl_ro_lc_249, 2912);
NFTL_RO_VIEW(nftl_ro_lc_250, 2984);
NFTL_RO_VIEW(nftl_ro_lc_251, 3024);
NFTL_RO_VIEW(nftl_ro_lc_252, 3056);
NFTL_RO_VIEW(nftl_ro_lc_253, 3080);
NFTL_RO_VIEW(nftl_ro_lc_254, 3128);
NFTL_RO_VIEW(nftl_ro_lc_255, 3176);
NFTL_RO_VIEW(nftl_ro_lc_256, 3216);
NFTL_RO_VIEW(nftl_ro_lc_257, 3256);
NFTL_RO_VIEW(nftl_ro_lc_258, 3344);
NFTL_RO_VIEW(nftl_ro_lc_259, 3432);
NFTL_RO_VIEW(nftl_ro_lc_260, 3472);
NFTL_RO_VIEW(nftl_ro_lc_261, 3504);
NFTL_RO_VIEW(nftl_ro_lc_262, 3536);
NFTL_RO_VIEW(nftl_ro_lc_263, 3592);
NFTL_RO_VIEW(nftl_ro_lc_264, 3632);
NFTL_RO_VIEW(nftl_ro_lc_265, 3696);
NFTL_RO_VIEW(nftl_ro_lc_266, 3728);
NFTL_RO_VIEW(nftl_ro_lc_267, 3760);
NFTL_RO_VIEW(nftl_ro_lc_268, 3816);
NFTL_RO_VIEW(nftl_ro_lc_269, 3880);
NFTL_RO_VIEW(nftl_ro_lc_270, 3936);
NFTL_RO_VIEW(nftl_ro_lc_271, 3992);
NFTL_RO_VIEW(nftl_ro_lc_272, 4024);
NFTL_RO_VIEW(nftl_ro_lc_273, 4080);
NFTL_RO_VIEW(nftl_ro_lc_274, 4112);
NFTL_RO_VIEW(nftl_ro_lc_275, 4168);
NFTL_RO_VIEW(nftl_ro_lc_276, 4232);
NFTL_RO_VIEW(nftl_ro_lc_277, 4288);
NFTL_RO_VIEW(nftl_ro_lc_278, 4336);
NFTL_RO_VIEW(nftl_ro_lc_279, 4384);
NFTL_RO_VIEW(nftl_ro_lc_280, 4400);
NFTL_RO_VIEW(nftl_ro_lc_281, 4416);
NFTL_RO_VIEW(nftl_ro_lc_282, 4432);
NFTL_RO_VIEW(nftl_ro_lc_283, 4448);
NFTL_RO_VIEW(nftl_ro_lc_284, 4488);
NFTL_RO_VIEW(nftl_ro_lc_285, 4536);
NFTL_RO_VIEW(nftl_ro_lc_286, 4592);
NFTL_RO_VIEW(nftl_ro_lc_287, 4640);
NFTL_RO_VIEW(nftl_ro_lc_288, 4704);
NFTL_RO_VIEW(nftl_ro_lc_289, 4752);
NFTL_RO_VIEW(nftl_ro_lc_290, 4792);
NFTL_RO_VIEW(nftl_ro_lc_291, 4832);
NFTL_RO_VIEW(nftl_ro_lc_292, 4864);
NFTL_RO_VIEW(nftl_ro_lc_293, 4904);
NFTL_RO_VIEW(nftl_ro_lc_294, 4928);
NFTL_RO_VIEW(nftl_ro_lc_295, 4952);
NFTL_RO_VIEW(nftl_ro_lc_296, 5000);
NFTL_RO_VIEW(nftl_ro_lc_297, 5032);
NFTL_RO_VIEW(nftl_ro_lc_298, 5080);
NFTL_RO_VIEW(nftl_ro_lc_299, 5144);
NFTL_RO_VIEW(nftl_ro_lc_300, 5176);
NFTL_RO_VIEW(nftl_ro_lc_301, 5208);
NFTL_RO_VIEW(nftl_ro_lc_302, 5232);
NFTL_RO_VIEW(nftl_ro_lc_303, 5264);
NFTL_RO_VIEW(nftl_ro_lc_304, 5296);
NFTL_RO_VIEW(nftl_ro_lc_305, 5352);
NFTL_RO_VIEW(nftl_ro_lc_306, 5408);
NFTL_RO_VIEW(nftl_ro_lc_307, 5448);
NFTL_RO_VIEW(nftl_ro_lc_308, 5504);
NFTL_RO_VIEW(nftl_ro_lc_309, 5536);
NFTL_RO_VIEW(nftl_ro_lc_310, 5640);
NFTL_RO_VIEW(nftl_ro_lc_311, 5672);
NFTL_RO_VIEW(nftl_ro_lc_312, 5704);
NFTL_RO_VIEW(nftl_ro_lc_313, 5768);
NFTL_RO_VIEW(nftl_ro_lc_314, 5816);
NFTL_RO_VIEW(nftl_ro_lc_315, 5856);
NFTL_RO_VIEW(nftl_ro_lc_316, 5888);
NFTL_RO_VIEW(nftl_ro_lc_317, 5928);
NFTL_RO_VIEW(nftl_ro_lc_318, 5968);
NFTL_RO_VIEW(nftl_ro_lc_319, 6024);
NFTL_RO_VIEW(nftl_ro_lc_320, 6080);
NFTL_RO_VIEW(nftl_ro_lc_321, 6128);
NFTL_RO_VIEW(nftl_ro_lc_322, 6168);
NFTL_RO_VIEW(nftl_ro_lc_323, 6192);
NFTL_RO_VIEW(nftl_ro_lc_324, 6216);
NFTL_RO_VIEW(nftl_ro_lc_325, 6272);
NFTL_RO_VIEW(nftl_ro_lc_326, 6320);
NFTL_RO_VIEW(nftl_ro_lc_327, 6360);
NFTL_RO_VIEW(nftl_ro_lc_328, 6392);
NFTL_RO_VIEW(nftl_ro_lc_329, 6424);
NFTL_RO_VIEW(nftl_ro_lc_330, 6440);
NFTL_RO_VIEW(nftl_ro_lc_331, 6480);
NFTL_RO_VIEW(nftl_ro_lc_332, 6528);
NFTL_RO_VIEW(nftl_ro_lc_333, 6576);
NFTL_RO_VIEW(nftl_ro_lc_334, 6624);
NFTL_RO_VIEW(nftl_ro_lc_335, 6672);
NFTL_RO_VIEW(nftl_ro_lc_336, 6696);
NFTL_RO_VIEW(nftl_ro_lc_337, 6720);
NFTL_RO_VIEW(nftl_ro_lc_338, 6752);
NFTL_RO_VIEW(nftl_ro_lc_339, 6776);
NFTL_RO_VIEW(nftl_ro_lc_340, 6808);
NFTL_RO_VIEW(nftl_ro_lc_341, 6848);
NFTL_RO_VIEW(nftl_ro_lc_342, 6888);
NFTL_RO_VIEW(nftl_ro_lc_343, 6920);
NFTL_RO_VIEW(nftl_ro_lc_344, 6976);
NFTL_RO_VIEW(nftl_ro_lc_345, 7024);
NFTL_RO_VIEW(nftl_ro_lc_346, 7064);
NFTL_RO_VIEW(nftl_ro_lc_347, 7104);
NFTL_RO_VIEW(nftl_ro_lc_348, 7144);
NFTL_RO_VIEW(nftl_ro_lc_349, 7208);
NFTL_RO_VIEW(nftl_ro_lc_350, 7264);
NFTL_RO_VIEW(nftl_ro_lc_351, 7312);
NFTL_RO_VIEW(nftl_ro_lc_352, 7352);
NFTL_RO_VIEW(nftl_ro_lc_353, 7400);
NFTL_RO_VIEW(nftl_ro_lc_354, 7464);
NFTL_RO_VIEW(nftl_ro_lc_355, 7504);
NFTL_RO_VIEW(nftl_ro_lc_356, 7568);
NFTL_RO_VIEW(nftl_ro_lc_357, 7608);
NFTL_RO_VIEW(nftl_ro_lc_358, 7648);
NFTL_RO_VIEW(nftl_ro_lc_359, 7688);
NFTL_RO_VIEW(nftl_ro_lc_360, 7752);
NFTL_RO_VIEW(nftl_ro_lc_361, 7792);
NFTL_RO_VIEW(nftl_ro_lc_362, 7856);
NFTL_RO_VIEW(nftl_ro_lc_363, 7896);
NFTL_RO_VIEW(nftl_ro_lc_364, 7928);
NFTL_RO_VIEW(nftl_ro_lc_365, 7960);
NFTL_RO_VIEW(nftl_ro_lc_366, 8000);
NFTL_RO_VIEW(nftl_ro_lc_367, 8032);
NFTL_RO_VIEW(nftl_ro_lc_368, 8064);
NFTL_RO_VIEW(nftl_ro_lc_369, 8112);
NFTL_RO_VIEW(nftl_ro_lc_370, 8160);
NFTL_RO_VIEW(nftl_ro_lc_371, 8216);
NFTL_RO_VIEW(nftl_ro_lc_372, 8264);
NFTL_RO_VIEW(nftl_ro_lc_373, 8304);
NFTL_RO_VIEW(nftl_ro_lc_374, 8352);
NFTL_RO_VIEW(nftl_ro_lc_375, 8408);
NFTL_RO_VIEW(nftl_ro_lc_376, 8456);
NFTL_RO_VIEW(nftl_ro_lc_377, 8512);
NFTL_RO_VIEW(nftl_ro_lc_378, 8576);
NFTL_RO_VIEW(nftl_ro_lc_379, 8616);
NFTL_RO_VIEW(nftl_ro_lc_380, 8664);
NFTL_RO_VIEW(nftl_ro_lc_381, 8704);
NFTL_RO_VIEW(nftl_ro_lc_382, 8752);
NFTL_RO_VIEW(nftl_ro_lc_383, 8784);
NFTL_RO_VIEW(nftl_ro_lc_384, 8848);
NFTL_RO_VIEW(nftl_ro_lc_385, 8896);
NFTL_RO_VIEW(nftl_ro_lc_386, 8928);
NFTL_RO_VIEW(nftl_ro_lc_387, 8960);
NFTL_RO_VIEW(nftl_ro_lc_388, 8992);
NFTL_RO_VIEW(nftl_ro_lc_389, 9024);
NFTL_RO_VIEW(nftl_ro_lc_390, 9056);
NFTL_RO_VIEW(nftl_ro_lc_391, 9136);
NFTL_RO_VIEW(nftl_ro_lc_392, 9168);
NFTL_RO_VIEW(nftl_ro_lc_393, 9200);
NFTL_RO_VIEW(nftl_ro_lc_394, 9232);
NFTL_RO_VIEW(nftl_ro_lc_395, 9240);
NFTL_RO_VIEW(nftl_ro_lc_396, 9272);
NFTL_RO_VIEW(nftl_ro_lc_397, 9296);
NFTL_RO_VIEW(nftl_ro_lc_398, 9320);
NFTL_RO_VIEW(nftl_ro_lc_399, 9344);
NFTL_RO_VIEW(nftl_ro_lc_141, 9360);
NFTL_RO_VIEW(nftl_ro_lc_142, 9400);
NFTL_RO_VIEW(nftl_ro_lc_143, 9432);
NFTL_RO_VIEW(nftl_ro_lc_144, 9464);
NFTL_RO_VIEW(nftl_ro_lc_145, 9496);
NFTL_RO_VIEW(nftl_ro_lc_470, 9536);
NFTL_RO_VIEW(nftl_ro_lc_471, 9584);
NFTL_RO_VIEW(nftl_ro_lc_472, 9632);
NFTL_RO_VIEW(nftl_ro_lc_473, 9680);
NFTL_RO_VIEW(nftl_ro_lc_176, 9736);
NFTL_RO_VIEW(nftl_ro_lc_177, 9760);
NFTL_RO_VIEW(nftl_ro_lc_178, 9808);
NFTL_RO_VIEW(nftl_ro_lc_179, 9856);
NFTL_RO_VIEW(nftl_ro_lc_180, 9896);
NFTL_RO_VIEW(nftl_ro_lc_181, 9944);
NFTL_RO_VIEW(nftl_ro_lc_182, 9992);
NFTL_RO_VIEW(nftl_ro_lc_183, 10040);
NFTL_RO_VIEW(nftl_ro_lc_184, 10088);
NFTL_RO_VIEW(nftl_ro_lc_185, 10120);
NFTL_RO_VIEW(nftl_ro_lc_186, 10152);
NFTL_RO_VIEW(nftl_ro_lc_187, 10184);
NFTL_RO_VIEW(nftl_ro_lc_188, 10224);
NFTL_RO_VIEW(nftl_ro_lc_189, 10272);
NFTL_RO_VIEW(nftl_ro_lc_190, 10320);
NFTL_RO_VIEW(nftl_ro_lc_191, 10368);
NFTL_RO_VIEW(nftl_ro_lc_192, 10416);
NFTL_RO_VIEW(nftl_ro_lc_193, 10456);
NFTL_RO_VIEW(nftl_ro_lc_194, 10496);
NFTL_RO_VIEW(nftl_ro_lc_195, 10536);
NFTL_RO_VIEW(nftl_ro_lc_196, 10576);
NFTL_RO_VIEW(nftl_ro_lc_197, 10616);
NFTL_RO_VIEW(nftl_ro_lc_198, 10656);
NFTL_RO_VIEW(nftl_ro_lc_199, 10696);
NFTL_RO_VIEW(nftl_ro_lc_146, 11248);
NFTL_RO_VIEW(nftl_ro_lc_147, 11304);
NFTL_RO_VIEW(nftl_ro_lc_148, 11336);
NFTL_RO_VIEW(nftl_ro_lc_149, 11376);
NFTL_RO_VIEW(nftl_ro_lc_150, 11400);
NFTL_RO_VIEW(nftl_ro_lc_151, 11440);
NFTL_RO_VIEW(nftl_ro_lc_152, 11496);
NFTL_RO_VIEW(nftl_ro_lc_153, 11528);
NFTL_RO_VIEW(nftl_ro_lc_154, 11560);
NFTL_RO_VIEW(nftl_ro_lc_155, 11592);
NFTL_RO_VIEW(nftl_ro_lc_156, 11624);
NFTL_RO_VIEW(nftl_ro_lc_157, 11664);
NFTL_RO_VIEW(nftl_ro_lc_158, 11704);
NFTL_RO_VIEW(nftl_ro_lc_159, 11752);
NFTL_RO_VIEW(nftl_ro_lc_160, 11792);
NFTL_RO_VIEW(nftl_ro_lc_161, 11832);
NFTL_RO_VIEW(nftl_ro_lc_162, 11880);
NFTL_RO_VIEW(nftl_ro_lc_163, 11928);
NFTL_RO_VIEW(nftl_ro_lc_164, 11984);
NFTL_RO_VIEW(nftl_ro_lc_165, 12040);
NFTL_RO_VIEW(nftl_ro_lc_166, 12072);
NFTL_RO_VIEW(nftl_ro_lc_167, 12120);
NFTL_RO_VIEW(nftl_ro_lc_168, 12152);
NFTL_RO_VIEW(nftl_ro_lc_169, 12248);
NFTL_RO_VIEW(nftl_ro_lc_170, 12296);
NFTL_RO_VIEW(nftl_ro_lc_171, 12392);
NFTL_RO_VIEW(nftl_ro_lc_172, 12440);
NFTL_RO_VIEW(nftl_ro_lc_173, 12512);
NFTL_RO_VIEW(nftl_ro_lc_174, 12536);
NFTL_RO_VIEW(nftl_ro_lc_175, 12584);
NFTL_RO_VIEW(nftl_ro_lc_413, 12640);
NFTL_RO_VIEW(nftl_ro_lc_414, 12680);
NFTL_RO_VIEW(nftl_ro_lc_415, 12720);
NFTL_RO_VIEW(nftl_ro_lc_416, 12752);
NFTL_RO_VIEW(nftl_ro_lc_417, 12784);
NFTL_RO_VIEW(nftl_ro_lc_418, 12824);
NFTL_RO_VIEW(nftl_ro_lc_419, 12872);
NFTL_RO_VIEW(nftl_ro_lc_420, 12920);
NFTL_RO_VIEW(nftl_ro_lc_421, 12952);
NFTL_RO_VIEW(nftl_ro_lc_422, 12976);
NFTL_RO_VIEW(nftl_ro_lc_423, 13016);
NFTL_RO_VIEW(nftl_ro_lc_424, 13056);
NFTL_RO_VIEW(nftl_ro_lc_425, 13096);
NFTL_RO_VIEW(nftl_ro_lc_426, 13136);
NFTL_RO_VIEW(nftl_ro_lc_427, 13184);
NFTL_RO_VIEW(nftl_ro_lc_428, 13232);
NFTL_RO_VIEW(nftl_ro_lc_429, 13272);
NFTL_RO_VIEW(nftl_ro_lc_430, 13296);
NFTL_RO_VIEW(nftl_ro_lc_431, 13336);
NFTL_RO_VIEW(nftl_ro_lc_432, 13360);
NFTL_RO_VIEW(nftl_ro_lc_474, 13384);
NFTL_RO_VIEW(nftl_ro_lc_475, 13416);
NFTL_RO_VIEW(nftl_ro_lc_476, 13456);
NFTL_RO_VIEW(nftl_ro_lc_477, 13504);
NFTL_RO_VIEW(nftl_ro_lc_478, 13536);
NFTL_RO_VIEW(nftl_ro_lc_479, 13560);
NFTL_RO_VIEW(nftl_ro_lc_480, 13592);
NFTL_RO_VIEW(nftl_ro_lc_481, 13632);
NFTL_RO_VIEW(nftl_ro_lc_482, 13664);
NFTL_RO_VIEW(nftl_ro_lc_433, 13680);
NFTL_RO_VIEW(nftl_ro_lc_434, 13720);
NFTL_RO_VIEW(nftl_ro_lc_435, 13760);
NFTL_RO_VIEW(nftl_ro_lc_436, 13800);
NFTL_RO_VIEW(nftl_ro_lc_437, 13840);
NFTL_RO_VIEW(nftl_ro_lc_438, 13880);
NFTL_RO_VIEW(nftl_ro_lc_439, 13952);
NFTL_RO_VIEW(nftl_ro_lc_440, 14056);
NFTL_RO_VIEW(nftl_ro_lc_441, 14104);
NFTL_RO_VIEW(nftl_ro_lc_442, 14168);
NFTL_RO_VIEW(nftl_ro_lc_443, 14224);
NFTL_RO_VIEW(nftl_ro_lc_444, 14304);
NFTL_RO_VIEW(nftl_ro_lc_445, 14360);
NFTL_RO_VIEW(nftl_ro_lc_446, 14408);
NFTL_RO_VIEW(nftl_ro_lc_447, 14440);
NFTL_RO_VIEW(nftl_ro_lc_448, 14472);
NFTL_RO_VIEW(nftl_ro_lc_449, 14504);
NFTL_RO_VIEW(nftl_ro_lc_450, 14560);
NFTL_RO_VIEW(nftl_ro_lc_451, 14592);
NFTL_RO_VIEW(nftl_ro_lc_452, 14624);
NFTL_RO_VIEW(nftl_ro_lc_453, 14664);
NFTL_RO_VIEW(nftl_ro_lc_454, 14696);
NFTL_RO_VIEW(nftl_ro_lc_455, 14728);
NFTL_RO_VIEW(nftl_ro_lc_456, 14792);
NFTL_RO_VIEW(nftl_ro_lc_457, 14864);
NFTL_RO_VIEW(nftl_ro_lc_458, 14904);
NFTL_RO_VIEW(nftl_ro_lc_459, 14936);
NFTL_RO_VIEW(nftl_ro_lc_460, 14984);
NFTL_RO_VIEW(nftl_ro_lc_461, 15008);
NFTL_RO_VIEW(nftl_ro_lc_462, 15048);
NFTL_RO_VIEW(nftl_ro_lc_463, 15096);
NFTL_RO_VIEW(nftl_ro_lc_464, 15128);
NFTL_RO_VIEW(nftl_ro_lc_465, 15152);
NFTL_RO_VIEW(nftl_ro_lc_466, 15176);
NFTL_RO_VIEW(nftl_ro_lc_467, 15216);
NFTL_RO_VIEW(nftl_ro_lc_468, 15256);
NFTL_RO_VIEW(nftl_ro_lc_469, 15288);
NFTL_RO_VIEW(nftl_ro_lc_18, 15304);
NFTL_RO_VIEW(nftl_ro_lc_19, 15344);
NFTL_RO_VIEW(nftl_ro_lc_20, 15384);
NFTL_RO_VIEW(nftl_ro_lc_21, 15416);
NFTL_RO_VIEW(nftl_ro_lc_22, 15456);
NFTL_RO_VIEW(nftl_ro_lc_23, 15536);
NFTL_RO_VIEW(nftl_ro_lc_24, 15600);
NFTL_RO_VIEW(nftl_ro_lc_25, 15648);
NFTL_RO_VIEW(nftl_ro_lc_26, 15696);
NFTL_RO_VIEW(nftl_ro_lc_27, 15728);
NFTL_RO_VIEW(nftl_ro_lc_28, 15768);
NFTL_RO_VIEW(nftl_ro_lc_29, 15816);
NFTL_RO_VIEW(nftl_ro_lc_30, 15856);
NFTL_RO_VIEW(nftl_ro_lc_31, 15888);
NFTL_RO_VIEW(nftl_ro_lc_32, 15936);
NFTL_RO_VIEW(nftl_ro_lc_33, 15968);
NFTL_RO_VIEW(nftl_ro_lc_34, 16016);
NFTL_RO_VIEW(nftl_ro_lc_35, 16064);
NFTL_RO_VIEW(nftl_ro_lc_107, 16088);
NFTL_RO_VIEW(nftl_ro_lc_108, 16120);
NFTL_RO_VIEW(nftl_ro_lc_109, 16168);
NFTL_RO_VIEW(nftl_ro_lc_110, 16200);
NFTL_RO_VIEW(nftl_ro_lc_111, 16216);
NFTL_RO_VIEW(nftl_ro_lc_104, 16296);
NFTL_RO_VIEW(nftl_ro_lc_112, 16376);
NFTL_RO_VIEW(nftl_ro_lc_113, 16400);
NFTL_RO_VIEW(nftl_ro_lc_114, 16456);
NFTL_RO_VIEW(nftl_ro_lc_115, 16480);
NFTL_RO_VIEW(nftl_ro_lc_116, 16504);
NFTL_RO_VIEW(nftl_ro_lc_117, 16560);
NFTL_RO_VIEW(nftl_ro_lc_118, 16576);
NFTL_RO_VIEW(nftl_ro_lc_119, 16632);
NFTL_RO_VIEW(nftl_ro_lc_120, 16648);
NFTL_RO_VIEW(nftl_ro_lc_121, 16712);
NFTL_RO_VIEW(nftl_ro_lc_123, 16784);
NFTL_RO_VIEW(nftl_ro_lc_124, 16840);
NFTL_RO_VIEW(nftl_ro_lc_51, 16872);
NFTL_RO_VIEW(nftl_ro_lc_125, 16920);
NFTL_RO_VIEW(nftl_ro_lc_126, 16968);
NFTL_RO_VIEW(nftl_ro_lc_127, 17008);
NFTL_RO_VIEW(nftl_ro_lc_54, 17072);
NFTL_RO_VIEW(nftl_ro_lc_56, 17112);
NFTL_RO_VIEW(nftl_ro_lc_57, 17144);
NFTL_RO_VIEW(nftl_ro_lc_59, 17152);
NFTL_RO_VIEW(nftl_ro_lc_128, 17200);
NFTL_RO_VIEW(nftl_ro_lc_61, 17256);
NFTL_RO_VIEW(nftl_ro_lc_62, 17264);
NFTL_RO_VIEW(nftl_ro_lc_129, 17352);
NFTL_RO_VIEW(nftl_ro_lc_64, 17400);
NFTL_RO_VIEW(nftl_ro_lc_130, 17456);
NFTL_RO_VIEW(nftl_ro_lc_131, 17488);
NFTL_RO_VIEW(nftl_ro_lc_132, 17608);
NFTL_RO_VIEW(nftl_ro_lc_36, 17768);
NFTL_RO_VIEW(nftl_ro_lc_37, 17808);
NFTL_RO_VIEW(nftl_ro_lc_38, 17824);
NFTL_RO_VIEW(nftl_ro_lc_39, 17912);
NFTL_RO_VIEW(nftl_ro_lc_40, 17944);
NFTL_RO_VIEW(nftl_ro_lc_41, 18024);
NFTL_RO_VIEW(nftl_ro_lc_85, 18248);
NFTL_RO_VIEW(nftl_ro_lc_44, 18272);
NFTL_RO_VIEW(nftl_ro_lc_86, 18280);
NFTL_RO_VIEW(nftl_ro_lc_87, 18296);
NFTL_RO_VIEW(nftl_ro_lc_89, 18408);
NFTL_RO_VIEW(nftl_ro_lc_90, 18488);
NFTL_RO_VIEW(nftl_ro_lc_91, 18520);
NFTL_RO_VIEW(nftl_ro_lc_92, 18640);
NFTL_RO_VIEW(nftl_ro_lc_98, 19144);
NFTL_RO_VIEW(nftl_ro_lc_99, 19184);
NFTL_RO_VIEW(nftl_ro_lc_100, 19224);
NFTL_RO_VIEW(nftl_ro_lc_101, 19352);
NFTL_RO_VIEW(nftl_ro_lc_102, 19488);
NFTL_RO_VIEW(nftl_ro_lc_103, 19544);
NFTL_RO_VIEW(nftl_ro_lc_105, 19576);
NFTL_RO_VIEW(nftl_ro_lc_106, 19600);
NFTL_RO_VIEW(nftl_ro_lc_77, 19624);
NFTL_RO_VIEW(nftl_ro_lc_78, 19664);
NFTL_RO_VIEW(nftl_ro_lc_79, 19704);
NFTL_RO_VIEW(nftl_ro_lc_80, 19720);
NFTL_RO_VIEW(nftl_ro_lc_81, 19808);
NFTL_RO_VIEW(nftl_ro_lc_82, 19840);
NFTL_RO_VIEW(nftl_ro_lc_83, 19952);
NFTL_RO_VIEW(nftl_ro_lc_69, 20096);
NFTL_RO_VIEW(nftl_ro_lc_70, 20160);
NFTL_RO_VIEW(nftl_ro_lc_71, 20176);
NFTL_RO_VIEW(nftl_ro_lc_72, 20264);
NFTL_RO_VIEW(nftl_ro_lc_73, 20304);
NFTL_RO_VIEW(nftl_ro_lc_74, 20336);
NFTL_RO_VIEW(nftl_ro_lc_75, 20416);
NFTL_RO_VIEW(nftl_ro_lc_134, 20528);
NFTL_RO_VIEW(nftl_ro_lc_135, 20544);
NFTL_RO_VIEW(nftl_ro_lc_136, 20632);
NFTL_RO_VIEW(nftl_ro_lc_137, 20688);
NFTL_RO_VIEW(nftl_ro_lc_138, 20720);
NFTL_RO_VIEW(nftl_ro_lc_139, 20832);
NFTL_RO_VIEW(nftl_ro_lc_43, 20976);
NFTL_RO_VIEW(nftl_ro_lc_46, 20992);
NFTL_RO_VIEW(nftl_ro_lc_48, 21016);
NFTL_RO_VIEW(nftl_ro_lc_49, 21040);
NFTL_RO_VIEW(nftl_ro_lc_52, 21152);
NFTL_RO_VIEW(nftl_ro_lc_53, 21176);
NFTL_RO_VIEW(nftl_ro_lc_55, 21200);
NFTL_RO_VIEW(nftl_ro_lc_58, 21240);
NFTL_RO_VIEW(nftl_ro_lc_60, 21280);
NFTL_RO_VIEW(nftl_ro_lc_63, 21336);
NFTL_RO_VIEW(nftl_ro_lc_65, 21376);
NFTL_RO_VIEW(nftl_ro_lc_66, 21408);
NFTL_RO_VIEW(nftl_ro_lc_67, 21528);

/* Original compiler-generated function names also live in this pool. */
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_5346) == 21688, "NFTL func_5346 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_5194) == 21704, "NFTL func_5194 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7098) == 21720, "NFTL func_7098 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7312) == 21736, "NFTL func_7312 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7402) == 21752, "NFTL func_7402 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7526) == 21776, "NFTL func_7526 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7558) == 21808, "NFTL func_7558 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7569) == 21832, "NFTL func_7569 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7580) == 21856, "NFTL func_7580 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7589) == 21880, "NFTL func_7589 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7642) == 21904, "NFTL func_7642 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, function_7666) == 21928, "NFTL function_7666 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_6755) == 21960, "NFTL func_6755 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_8479) == 21984, "NFTL func_8479 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_7884) == 22008, "NFTL func_7884 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_8536) == 22024, "NFTL func_8536 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_8331) == 22040, "NFTL func_8331 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_5524) == 22056, "NFTL func_5524 offset");
_Static_assert(__builtin_offsetof(struct nftl_readonly_layout, func_6409) == 22080, "NFTL func_6409 offset");
NFTL_RO_VIEW(nftl_ro_func_5346, 21688);
NFTL_RO_VIEW(nftl_ro_func_5194, 21704);
NFTL_RO_VIEW(nftl_ro_func_7098, 21720);
NFTL_RO_VIEW(nftl_ro_func_7312, 21736);
NFTL_RO_VIEW(nftl_ro_func_7402, 21752);
NFTL_RO_VIEW(nftl_ro_func_7526, 21776);
NFTL_RO_VIEW(nftl_ro_func_7558, 21808);
NFTL_RO_VIEW(nftl_ro_func_7569, 21832);
NFTL_RO_VIEW(nftl_ro_func_7580, 21856);
NFTL_RO_VIEW(nftl_ro_func_7589, 21880);
NFTL_RO_VIEW(nftl_ro_func_7642, 21904);
NFTL_RO_VIEW(nftl_ro_function_7666, 21928);
NFTL_RO_VIEW(nftl_ro_func_6755, 21960);
NFTL_RO_VIEW(nftl_ro_func_8479, 21984);
NFTL_RO_VIEW(nftl_ro_func_7884, 22008);
NFTL_RO_VIEW(nftl_ro_func_8536, 22024);
NFTL_RO_VIEW(nftl_ro_func_8331, 22040);
NFTL_RO_VIEW(nftl_ro_func_5524, 22056);
NFTL_RO_VIEW(nftl_ro_func_6409, 22080);
#undef NFTL_RO_VIEW
