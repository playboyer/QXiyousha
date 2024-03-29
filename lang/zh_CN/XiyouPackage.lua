-- translation for ShadowPackage

return {
	["xiyou"] = "西游杀",

    --佛阵营
	--唐僧
	["tangseng"] = "唐三藏",
	["tuxi"] = "化缘",
	[":tuxi"] = "摸牌阶段，你可以选择放弃摸牌，然后从两名角色的手牌里各抽取一张牌，或者少摸一张牌然后从一名角色的手牌里抽取一张牌。",
	["@tuxi-card"] = "你是否想发动【化缘】技能",
	["hujia"] = "护法",
	[":hujia"] = "<b>专属技</b>，当你需要使用（或打出）一张【遁】时，你可以发动护驾。所有佛势力角色按行动顺序依次选择是否打出一张【遁】提供给你（视为由你使用或打出），直到有一名角色或没有任何角色决定如此做时为止。",
	["hujia:yes"] = "所有魏势力角色按行动顺序依次选择是否打出一张【遁】提供给你（视为由你使用或打出），直到有一名角色或没有任何角色决定如此做时为止",
	[":hujia:"] = "%from 想让你帮他出一张【遁】：",
	["hujia:accept"] = "帮他护法",
	["hujia:ignore"] = "拒绝护法",
	["@hujia-jink"] = "请打出一张【遁】以响应【护法】",

 	--孙悟空
	["sunwukong"] = "孙悟空",
	["ruyibang"] = "如意棒",
	[":ruyibang"] = "当你指定一名角色为【劫】的目标时，你可以额外指定该角色的下家也成为此【劫】的目标。",
	["qishier"] = "七十二变",
	[":qishier"] = "出牌阶段，你可以弃一张红色手牌，变身为场上任意一名角色。回合结束阶变回真身。",
	["jindou"] = "筋斗云",
	[":jindou"] = "出牌阶段，你可以弃一张黑色手牌，与场上任意一名角色交换位置。",
	["#Ruyibang"] = "%from 的技能<font color='yellow'>【如意棒】</font>将 %arg 和 %to 同时成为了此<font color='yellow'>【劫】</font>的目标",
	["#Jindouyun"] = "%from 与 %to 交换了<font color='yellow'>位置</font>",

    --猪八戒
	["zhubajie"] = "猪八戒",
	["longdan"] = "天蓬",
	[":longdan"] = "你可以将你手牌的【水劫】当【遁】、【遁】当【水劫】使用或打出。\
	◆使用【天蓬】时，仅改变牌的类别（名称）和作用，而牌的花色和点数不变。",
	["jingtan"] = "净坛",
	[":jingtan"] = "出牌阶段：可收走任一目标的任意数量的法宝。若如此做，则你的元阳-1。",

	--沙悟净
	["shawujing"] = "沙悟净",
	["wusheng"] = "卷帘",
	[":wusheng"] = "你可以将你的任意一张黑色手牌当【水劫】使用或打出。【水劫】对你无效\
	◆使用【卷帘】时，仅改变牌的类别（名称）和作用，而牌的花色和点数不变。",
	["#Juanlian"] =
"%from 的<font color='yellow'><b>【%arg】</b></font>效果被触发，<font color='yellow'><b>【水劫】</b></font>对其无效",

	--如来佛祖
	["rulai"] = "如来佛祖",
	["wuzhishan"] = "五行山",
	[":wuzhishan"] = "出牌阶段，你可以将你的任意方块花色的牌当【五行山】使用。",
	["mingce"] = "佛旨",
	[":mingce"] = "出牌阶段，你可以交给任一其他角色一张装备牌或【劫】，该角色进行二选一：\
	1、视为对其攻击范围内的另一名由你指定的角色使用一张【劫】。\
	2、摸一张牌。\
	每回合限用一次。",
	["mingce:nothing"] = "收下此牌",
	["mingce:use"] = "视为对自己攻击范围内的另一名由 如来佛祖 指定的角色使用一张【劫】",
	["mingce:draw"] = "摸一张牌",

	--观音菩萨
	["guanyin"] = "观音菩萨",
	["yiji"] = "大慈大悲",
	[":yiji"] = "你每受到1点伤害，可以立刻摸两张牌单独或分别送给任意角色。",
	["jiuku"] = "救苦救难",
	[":jiuku"] = "回合外，每当场上除你外的角色受到【劫】的伤害，你可以弃一张红色手牌使得该【劫】造成的伤害无效。",
	["#Jiuku"] = "<font color='yellow'><b>观音菩萨</b></font> 发动了技能<font color='yellow'><b>【救苦救难】</b></font>，将造成的 %arg 点伤害改为无效",
	["@jiuku-ask"] = "请弃掉一张红色手牌",

	--弥勒佛
	["mile"] = "弥勒佛",
	["lianying"] = "笑口常开",
	[":lianying"] = "失去最后一张手牌时，可以立即摸一张。",
	["keji"] = "大肚能容",
	[":keji"] = "如回合内没有出【劫】，可以跳过弃牌。",

	--灵吉菩萨
	["lingji"] = "灵吉菩萨",
	["dingfeng"] = "定风",
	[":dingfeng"] = "每当你攻击范围内的角色对你使用非延时法术，你可以进行一次问卜：若为梅花或方块，则解除此法术对你的效果",
	["feilong"] = "飞龙",
	[":feilong"] = "没有装备武器时， 你的攻击距离为3",
	["$Feilong"] = "%from 在本回合内攻击范围为 <font color='yellow'><b>3</b></font>",
	["#Dingfeng"] = "%from 的技能<font color='yellow'><b>【定风】</b></font>解除了 %to 对其使用的法术效果",

	--仙阵营
	--玉皇大帝
	["yuhuang"] = "玉皇大帝",
	["rende"] = "天赐",
	[":rende"] = "出牌阶段，你可以将任意数量的手牌以任意分配方式交给其他角色，若你给出的牌张数不少于两张时，你回复1点元阳。\
	◆使用【天赐】分出的牌，对方无法拒绝。",
	["jijiang"] = "无极大道",
	[":jijiang"] = "<b>专属技</b>，当你需要使用（或打出）一张【劫】时，你可以发动激将。所有仙阵营角色按行动顺序依次选择是否打出一张【劫】提供给你（视为由你使用或打出），直到有一名角色或没有任何角色决定如此作时为止。",
	[":jijiang:"] = "%from 想让你帮其出一张【劫】：",
	["jijiang:accept"] = "响应无极大道",
	["jijiang:ignore"] = "拒绝响应",
	["@jijiang-slash"] = "请打出一张【劫】以响应【无极大道】",

	--王母娘娘
	["wangmu"] = "王母娘娘",
	["qingnang"] = "蟠桃",
	[":qingnang"] = "出牌阶段，你可以主动弃掉一张手牌，令任一目标角色回复1点体力。每回合限用一次。",
	["jijiu"] = "蟠桃大会",
	[":jijiu"] = "你的回合外，你可以将你的任意红色牌当【蟠桃】使用。",

	--杨戬
	["yangjian"] = "二郎神",
	["sanjiandao"] = "三尖刀",
	[":sanjiandao"] = "你使用【劫】时，目标角色需连续使用两张【遁】才能抵消；与你进行【决斗】的角色每次需连续打出两张【劫】\
★若对方只有一张【遁】或【劫】则即便使用（打出）了也无效",
    ["yjqishier"] = "七十二变",
	[":yjqishier"] = "出牌阶段，你可以弃一张红色手牌，变身为场上任意一名角色。回合结束阶段变回真身。",
	["tianyan"] = "天眼",
	[":tianyan"] = "出牌阶段，你可以弃一张牌，观看场上任意一名角色的手牌。",
	["@sanjiandao-slash-1"] = "%src 向您决斗，由于他有【三尖刀】技能，您必须打出两张【劫】",
	["@sanjiandao-slash-2"] = "%src 的决斗有【三尖刀】效果，请再打出一张【劫】",
	["@sanjiandao-jink-1"] = "%src 拥有【三尖刀】技能，您必须连续出两张【遁】",
	["@sanjiandao-jink-2"] = "%src 拥有【三尖刀】技能，你还需出一张【遁】",

	--哪吒
	["nezha"] = "哪吒",
	["santou"] = "三头",
	[":santou"] = "每回合可发动三次【劫】。",
	["liubi"] = "六臂",
	[":liubi"] = "你的手牌上限+1。",
	["$Santou"] = "%from 在本回合内可使用三次<font color='yellow'><b>【劫】</b></font>",
	["$Liubi"] = "%from 发动了<font color='yellow'><b>【六臂】</b></font>技能，手牌上限 <font color='yellow'><b>+1</b></font>",

	--菩提老祖
	["puti"] = "菩提老祖",
	["guanxing"] = "扶鸾",
	[":guanxing"] = "回合开始阶段，你可以观看牌堆顶的X张牌（X为存活角色的数量且最多为5），将其中任意数量的牌以任意顺序置于牌堆顶，其余以任意顺序置于牌堆底。",
	["#GuanxingResult"] = "%from 的<font color='yellow'><b>【扶鸾】</b></font>结果：%arg 张置于牌堆顶，%arg2 张置于牌堆底",
	["guicai"] = "问卜",
	[":guicai"] = "在任意角色的问卜牌亮出后，可以打一张手牌代替之。（生死簿除外）",
	["fankui:yes"] = "你将获得伤害来源的一张手牌或装备牌。",
	["@guicai-card"] = "请使用【问卜】修改 %src 的 %arg 问卜牌",

	--东海龙王
	["longwang"] = "东海龙王",
	["sihai"] = "四海之主",
	[":sihai"] = "每当场上有角色受到水属性伤害后，你可以选择一种颜色，然后你翻开牌堆顶的一张牌，若你选择的颜色和此牌的颜色一致，你获得此牌。【水劫】对你无效 ",
	["$Sihai"] =
"%from 猜测的<font color='yellow'><b>【颜色】</b></font>，与<font color='yellow'><b>%card</b></font>的颜色不相同",
    ["#Zhizhu"] =
"%from 的<font color='yellow'><b>【%arg】</b></font>效果被触发，<font color='yellow'><b>【水劫】</b></font>对其无效",
    ["red"] = "红色牌",
	["black"] = "黑色牌",

	--镇元大仙
	["zhenyuanzi"] = "镇元大仙",
	["jianxiong"] = "袖里乾坤",
	[":jianxiong"] = "立刻获得对你造成伤害的牌，若对目标造成伤害后，可获得目标已装备上的一件法宝",
	["jianxiong:yes"] = "立即获得使你受到伤害的那张牌",
	["#xiuli"] = "袖里乾坤",
	[":#xiuli"] = "立刻获得对你造成伤害的牌，若对目标造成伤害后，可获得目标已装备上的一件法宝",
	["#xiuli:yes"] = "可获得目标已装备上的一件法宝",
	["$Xiuli"] = "%from 发动<font color='yellow'><b>【袖里乾坤】</b></font>抽取了 %to 的 <font color='yellow'><b>%card</b></font>",

	--阎王
	["yanwang"] = "阎王",
	["guidao"] = "生死簿",
	[":guidao"] = "在任意一名角色的问卜牌生效前，你可以用自己的一张【黑桃】或【梅花】牌替换之。",
	["xingshang"] = "冥界之主",
	[":xingshang"] = "你可将【生死簿】无视规则的使用，每当有角色死亡，立即获得该角色的手牌和法宝",
	["shengsi"] = "冥界之主",
	["xingshang:yes"] = "你可以获得阵亡者的所有手牌和装备牌",
	["@guidao-card"] = "请使用【生死簿】修改 %src 的 %arg 判定",
	["#Shengsibu"] = "<font color='yellow'><b>【生死簿】</b></font>问卜结束，%from 将受到 %arg 点无来源伤害",

	--嫦娥
	["change"] = "嫦娥",
	["guanghan"] = "广寒",
    [":guanghan"]=
"当你受到伤害时，如果打出一张红色手牌，可将伤害转嫁给攻击范围内的其他角色（不能为伤害来源玩家）。",
	["@@guanghan-card"] = "你可以发动【广寒】弃一张红色手牌将伤害转给你能攻击到的另一名角色（不能为伤害来源角色）",

	--妖阵营
	--牛魔王
	["niumowang"] = "牛魔王",
	["tieji"] = "混世",
	[":tieji"] = "当你使用一张劫，你可以进行问卜，若结果为红色，则此【劫】不可用【遁】躲避，若问卜结果为黑色，则你可以弃掉目标角色一张手牌或装备区牌。",
	["tieji:yes"] = "可以进行一次问卜",

	--铁扇公主
	["tieshan"] = "铁扇公主",
	["bajiaoshan"] = "芭蕉扇",
	[":bajiaoshan"] = "当你使用的【劫】对目标角色造成伤害，你可以让该角色和其下家交换场上位置。【风劫】对你无效",
	["#Luocha"] =
"%from 的<font color='yellow'><b>【%arg】</b></font>效果被触发，<font color='yellow'><b>【风劫】</b></font>对其无效",
    ["#Baijiao"] = "%from 被 <font color='yellow'>芭蕉扇</font> 吹出了十万八千里",
	["#BajiaoEffect"] =
"%from 的武器<font color='yellow'><b>【芭蕉扇】</b></font>效果被触发，伤害从 %arg 点上升至 %arg2 点",

	--银角大王
	["yinjiao"] = "银角大王",
	["hulu"] = "紫金葫芦",
	[":hulu"] = "出牌阶段，你可以展示并弃掉一张手牌，然后抽取任意一名角色的手牌，若你所弃掉的展示牌与你抽取该角色的牌花色相同，则目标角色元阳-1。（每回合限用一次）",
	["$Hulu"] = "%from 弃掉了这张<font color='red'>展示牌</font>",
	["#Hulu"] = "%from 所弃掉的牌的<font color='yellow'><b>花色</b></font>与抽取 %to 的牌的<font color='yellow'><b>花色</b></font>相同",

	--白骨精
	["baigujing"] = "白骨精",
	["lijian"] = "离间",
	[":lijian"] = "出牌阶段，若你弃掉任意一张手牌，你可以离间两个目标：双方轮流出劫（指定一方先出），最先不能出劫者元阳-1（每回合使用一次）。",
	["jieshi"] = "解尸",
	[":jieshi"] = "每当你元阳减少一点，可立即摸一张牌。",

	--红孩儿
	["honghaier"] = "红孩儿",
	["sanmeizhenhuo"] = "三昧真火",
	[":sanmeizhenhuo"] =
"当你使用【火劫】时，造成的伤害+1。【火劫】对你无效",
    ["#ZhenhuoEffect"] =
"%from 的<font color='yellow'><b>【三昧真火】</b></font>效果被触发，伤害从 %arg 点上升至 %arg2 点",
    ["#Zhenhuo"] =
"%from 的<font color='yellow'><b>【%arg】</b></font>效果被触发，<font color='yellow'><b>【火劫】</b></font>对其无效",

	--金鼻白毛鼠
	["baimaoshu"] = "金鼻白毛鼠",
	["qiuyang"] = "姹女求阳",
	[":qiuyang"] = "出牌阶段，你可以抽取任一男性角色的一张手牌。（每回合限用一次）",
    ["tuoqiao"] = "金蚕脱壳",
	[":tuoqiao"] = "你可以把任意方块花色的牌当做遁使用或打出。",

	--蜘蛛精
    ["zhizhujing"]="蜘蛛精",
    ["fanjian"]="七情迷本",
    [":fanjian"]=
"可指定一名男性角色，该角色选择两种花色，抽取你的一张手牌，若花色不符，则交出一格元阳给你，无论结果如何，该角色都获得这张牌。（每回合使用一次）",
    ["#QiqingSuit"] = "%from 选择了 <font color='yellow'><b>%arg</b></font> 和 <font color='yellow'><b>%arg2</b></font> 花色",

	--九灵元圣
	["yuansheng"] = "九灵元圣",
	["jiutou"] = "九头狮子",
	[":jiutou"] = "每回合可发动九次【劫】。当使用的【劫】是最后一张手牌时，可为该【劫】指定九个目标。",
	["$Jiutou"] = "%from 在本回合内可使用九次<font color='yellow'><b>【劫】</b></font>",

	--人阵营
	["guowang"] = "女儿国王",
	["jieyin"] = "联姻",
	[":jieyin"] =
"出牌阶段，弃掉任意两张手牌可以和异性角色联姻。双方各回复一格元阳。每回合限用一次。\
	◆使用【联姻】的条件是场上有受伤的异性角色，与你自己是否受伤无关。",
	["jinguo"] = "巾帼",
	[":jinguo"] =
"回合结束时，你可以摸一张牌。",

	--武将台词
	--佛阵营
	["$tuxi"] = "贫僧随缘布施些斋吃~",
	["$hujia"] = "悟空救我！",

	["$ruyibang"] = "看我金箍棒横扫千军~",
	["$qishier"] = "我变！变！变~",
	["$jindou"] = "（呼~~）",

	["$longdan"] = "天蓬元帅下凡尘~",
	["$jingtan"] = "多多益善，多多益善~",

	["$wusheng"] = "我乃卷帘大将，吃我一仗！",

	["$wuzhishan"] = "泼猴~哪里走？",
	["$mingce"] = "南无阿弥陀佛~",

	["$yiji"] = "解八难，渡群生~",
	["$jiuku"] = "救苦寻声，万称万应，千圣千灵~",

	["$lianying"] = "极乐场中第一尊，南无弥勒笑和尚~",
	["$keji"] = "我大肚能容，不弃牌~",

	["$dingfeng"] = "千磨万击还坚劲，任尔东西南北风~",
	["$feilong"] = "须弥山有飞龙杖，灵吉当年受佛兵~",

	--仙阵营
	["$rende"] = "众位爱卿，可与朕同乐~",
	["$jijiang"] = "哪路神将下界收妖？",

	["$qingnang"] = "蟠桃~",
	["$jijiu"] = "蟠桃大会~",

	["$sanjiandao"] = "三尖两刃戟~",
	["$yjqishier"] = "变！变！变~",
	["$tianyan"] = "妖孽！哪里走？！",

	["$santou"] = "看我哪吒三头六臂的厉害！",
	["$liubi"] = "身带六般神器械，飞腾变化广无边~",

	["$guanxing"] = "请仙扶鸾，方知趋吉避凶之理~",
	["$guicai"] = "知凶定吉，断死言生~",

	["$jianxiong"] = "能收万物，袖里乾坤~",

	["$guidao"] = "阎王要你三更死，谁敢留人到五更？！",
	["$xingshang"] = "早死早超生，身外之物留给我吧~",

	["$sihai"] = "(海浪声~)",

	["$guanghan"] = "谁也伤不了我~",

	--妖阵营
	["$tieji"] = "四海有名称混世，西方大力号魔王！",

	["$bajiaoshan"] = "一扇挥动鬼神愁~",

	["$hulu"] = "我且叫你一声，你敢应吗？！",

	["$lijian"] = "先让我戏耍一番~",
	["$jieshi"] = "想杀本夫人？哼~",

	["$sanmeizhenhuo"] = "直挺火尖枪，喷火照四方！",

	["$qiuyang"] = "长老哥哥~请一杯交欢酒~",
	["$tuoqiao"] = "轻轻的，我走啦，哈哈哈~",

	["$fanjian"] = "妹妹们，不可怠慢，好好款待一下诸位长老~",

	["$jiutou"] = "（吼~）",

	--人阵营
	["$jieyin"] = "河边细品万里阳~",
	["$jinguo"] = "姐姐妹妹站起来~",

	--武将阵亡台词
	--佛阵营
	["~tangseng"] = "唐三藏·阵亡",
	["~sunwukong"] = "孙悟空·阵亡",
	["~zhubajie"] = "猪八戒·阵亡",
	["~shawujing"] = "沙悟净·阵亡",
	["~rulai"] = "如来佛祖·阵亡",
	["~guanyin"] = "观音菩萨·阵亡",
	["~mile"] = "弥勒佛·阵亡",
	["~lingji"] = "灵吉菩萨·阵亡",

	--仙阵营
	["~yuhuang"] = "玉皇大帝·阵亡",
	["~wangmu"] = "王母娘娘·阵亡",
	["~yangjian"] = "二郎神·阵亡",
	["~nezha"] = "哪吒·阵亡",
	["~puti"] = "菩提老祖·阵亡",
	["~longwang"] = "东海龙王·阵亡",
	["~zhenyuanzi"] = "镇元大仙·阵亡",
	["~yanwang"] = "阎王·阵亡",
	["~change"] = "嫦娥·阵亡",

	--妖阵营
	["~niumowang"] = "牛魔王·阵亡",
	["~tieshan"] = "铁扇公主·阵亡",
	["~yinjiao"] = "银角大王·阵亡",
    ["~baigujing"] = "白骨精·阵亡",
	["~honghaier"] = "红孩儿·阵亡",
	["~baimaoshu"] = "金鼻白毛鼠·阵亡",
	["~zhizhujing"] = "蜘蛛精·阵亡",
	["~yuansheng"] = "九灵元圣·阵亡",

	--人阵营
	["~guowang"] = "女儿国王·阵亡",

}
