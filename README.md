# MesTextTool，一个用于替换Mes脚本内文本的工具（马戏团引擎）。
## 0x0 输入参数
```log
# -LOG 输出日志（可选）
# -CP[xxx] 字符串的CodePage（可选）
# -GAME  指定游戏（可选）
# PATH Mes文件的目录或者需要导入文本的目录 （必须）

# 实例
MesTextTool.exe -log -cp932 -dc3wy D:\example\path\mes\

# 可将exe重命名加上参数
MesTextTool-log-cp932-dc3wy.exe D:\example\path\mes\
```
## 0x1 导出文本
将`mes文件`或者`文件夹`拖动到exe
```log
# 如需查看日志请使用控制台运行：
MesTextTool.exe -log D:\example\path\mes\
```
## 0x02 导入文本
首先确保你的文本目录下存在`.MesTextTool`这个文件，然后将整个文本目录拖动到exe上即可<br>
![Image text](https://github.com/cokkeijigen/MesTextTool/blob/master/picture.png)<br>
```log
# 如需查看日志请使用控制台运行：
MesTextTool.exe -log D:\example\path\text\
```
### `.MesTextTool`参数详细<br>
```
#InputPath ; 这个是源（未修改）mes文件的目录
D:\YourGames\Name\Advdata\MES

#UseCodePage ; 导入需要转换的编码(CodePage)
936 ; 默认： utf-8(65001) -> gbk(936)

#Text-MinLength ; 单行最小字数
22  ; 默认，当值为-1时则禁用自动文本格式化

#Text-MaxLength ; 单行最大字数
24  ; 同上

#Before-Replaces ; 格式化前替换文本
[]:[] ; 格式： [原来文本]:[替换文本] 支持替换多个，使用换行区分

#After-Replaces ; 格式化后替换文本
[]:[] ; 同上
```
如果想针对某一行不使用自动格式化，可以在前面加上`@::`，例如：
```
#0x1E4B
★◎  001  ◎★//@s現在の通常文字表示速度で表示しています。@s切り替えはオートに依存しています。
★◎  001  ◎★@::@s当前文字显示速度样本。（补丁仅供学习交流使\n@s用，严禁一切录播直播与商业行为）
```
***
默认情况下，mes的文本是以`sjis(cp932)`来解析。
如需指定编码，需要exe的重命名加上`-cp[xxxx]`，例如：

```
MesTextTool-cp936.exe ; cp936 -> GBK编码
```
程序默认是自动选择mes版本，**但不一定全都适用**，这时候就需要手动指定游戏版本了（<br>将exe重命名加上`-版本简称`，例如：
```
MesTextTool-cp936-dc4.exe ; 以GBK编码打开DC4版本的Mes文件
```
| 游戏| 简称 |
| :-----| :----: |
|恋夏 -れんげ-|renge|
|スペースライブ Space Live - Advent of the Net Idols|sl|
|T.P.さくら ～タイムパラディンさくら～ 前編|tpsk1|
|T.P.さくら ～タイムパラディンさくら～ 後編|tpsk2|
|紙ヒコーキ（まじかるぼっくす2009 祭・冬一番）|natuiro|
|ビスケット（まじかるぼっくす2009 祭・冬一番）|biscuit|
|ALBA ～Dawn of This Life～（まじかるぼっくす2009 祭・冬一番）|alba|
|めてぶれ（まじかるぼっくす2009 祭・冬一番）|mtbr|
|しまイチャ（まじかるぼっくす2009 祭・冬一番）|shimai|
|ダイスき（まじかるぼっくす2009 祭・冬一番）|diceki|
|はるか～Ｈａ・Ｒｕ・Ｋａ～（まじかるぼっくす2009 祭・冬一番）|hrk|
|TrueWorld～真実のセカイ～|tworld|
|舞-HiME 運命の系統樹 修羅|mhpc|
|SAKURA ～雪月華～|sakura|
|SAKURA ～雪月華～ SD版|sakurasd|
|Aries Pure Dream|ariespd|
|Aries Love Dream|ariesld|
|終の館 ～恋文～|tyakata1|
|終の館～双ツ星～|tyakata2|
|終の館～罪と罰～|tyakata3|
|終の館 ～檻姫～|tyakata4|
|終の館 ～人形～|tyakata5|
|水夏弐律 | suika2 |
|水夏A.S+ ～アズプラス～ | suikaas+ |
|水夏 ～SUIKAおー・157章～| suika |
|水夏 ～SUIKA～全年齢版 | suika |
|すくみず～フェチ☆になるもんっ！～|sukumizu|
|すくみず2 ～泳・げ・な・い～|sukumizu2|
|ガッデーム＆ジュテーム|gadejude|
|あるぴじ学園|arpg0|
|あるぴじ学園１．５|arpg1|
|A.G.II.D.C.　～あるぴじ学園2.0　サーカス史上最大の危機！？～|ag2dc|
|ホームメイド スイーツ| hmsw |
|ホームメイド -Home maid-| homemaid |
|ユニ uni.| uni |
|Princess Party ～プリンセスパーティー～| puripa |
|Princess Party Camellia ～プリンセスパーティーカメリア～|ccamellia|
|百花百狼/Hyakka Hyakurou|nightshade|
|てんぷれ！|tmpl|
|fortissimo//Akkord:Bsusvier | ffexa |
|fortissimo EXS//Akkord:nächsten Phase | ffexs |
|D.S.i.F. -Dal Segno-| dsif |
|Eternal Fantasy | ef |
|D.S. -Dal Segno- | ds |
|D.C.4 Plus Harmony 〜ダ・カーポ4〜 プラスハーモニー|dc4ph|
|D.C.4 ～ダ・カーポ4～ | dc4|
|D.C.III DreamDays|dc3dd|
|D.C.III WithYou|dc3wy|
|D.C.III RX-rated|dc3rx|
|D.C.II P.C.|dc2pc|
|D.C.II To You|dc2ty|
|D.C.II Spring Celebration|dc2sc|
|D.C.II C.C. 音姫先生のどきどき特別授業|dc2ccotm|
|D.C.II C.C. 月島小恋のらぶらぶバスルーム|dc2cckko|
|D.C.II 〜featuring　Yun2〜|dc2fy|
|D.C.II Dearest Marriage|dc2dm|
|D.C.II 春風のアルティメットバトル！|dc2bs, dcbs|
|D.C.II Fall in Love|dc2fl|
|D.C.A.S. 〜ダ・カーポ〜アフターシーズンズ|dcas|
|D.C. Dream X’mas|dcdx|
|Ｄ．Ｃ．Ｐ．Ｃ．(Vista)|dcpc|
|D.C. Summer Vacation|dcsv|
|D.C.WhiteSeason|dcws, dcws0|
|ことり Love Ex P|ktlep|
|D.C.〜ダ・カーポ〜　温泉編|dcxx|
|D.C.〜ダ・カーポ〜　MEMORIES DISC|dcxx, dcmems|
|C.D. Christmas Days ～サーカスディスク クリスマスデイズ～|cdcd|
|C.D.C.D.2 ～シーディーシーディー2～|cdcd2|
|CircusLand Ⅰ|cland1|

## 0x3 如何编译（Windows）
- **前提准备：**
**安装 [Visual Studio Community](https://visualstudio.microsoft.com/zh-hans/vs/community/) 和**
**[Cmake](https://cmake.org/download/)。**<br>
- **使用 `git clone https://github.com/cokkeijigen/MesTextTool` 克隆仓库，**
**或者下载 [MesTextTool-master.zip](https://codeload.github.com/cokkeijigen/MesTextTool/zip/refs/heads/master) 并解压。**<br>
- **打开`cmd`或者`poweshell`, `cd`到源码路径，运行`build.bat`**
```sh
git clone https://github.com/cokkeijigen/MesTextTool
cd MesTextTool
build.bat
```
**运行完`build.bat`，输出exe的路径为源码目录下`.\build\Release\MesTextTool.exe`**
