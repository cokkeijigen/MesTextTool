# MesTextTool，一个用于替换Mes脚本内文本的工具（马戏团引擎）。<br> A tool for replacing text within Mes scripts (Circus Engine).
## 0x0 输入参数 / Input Parameters
```log
# -LOG 输出日志（可选）/ Output log (Optional)
# -CP[xxx] 字符串的CodePage（可选）/ CodePage for strings (Optional)
# -GAME  指定游戏（可选）/ Specify game (Optional)
# PATH Mes文件的目录或者需要导入文本的目录 （必须）/ Directory of Mes files or text to import (Required)

# 实例 / Example
MesTextTool.exe -log -cp932 -dc3wy D:\example\path\mes\

# 可将exe重命名加上参数 / You can rename the exe to include parameters
MesTextTool-log-cp932-dc3wy.exe D:\example\path\mes\
```
## 0x1 导出文本 / Export Text
将`mes文件`或者`文件夹`拖动到exe <br>
Drag and drop the `mes` file or `folder` onto the executable.
```log
# 如需查看日志请使用控制台运行：
# To view logs, run via console:
MesTextTool.exe -log D:\example\path\mes\
```
## 0x02 导入文本
首先确保你的文本目录下存在`.MesTextTool`这个文件，然后将整个文本目录拖动到exe上即可<br>
First, ensure that the `.MesTextTool` file exists in your text directory, then drag the entire text directory onto the executable.<br>
![Image text](https://github.com/cokkeijigen/MesTextTool/blob/master/picture.png)<br>
```log
# 如需查看日志请使用控制台运行：
# To view logs, run via console:
MesTextTool.exe -log D:\example\path\text\
```
### `.MesTextTool` 参数详细 / Parameter Details <br>
```
#InputPath ; 这个是源（未修改）mes文件的目录 / Directory of the original (unmodified) mes files
D:\YourGames\Name\Advdata\MES

#UseCodePage ; 导入需要转换的编码(CodePage) / Encoding to convert during import (CodePage)
936 ; 默认： utf-8(65001) -> gbk(936)

#Text-MinLength ; 单行最小字数 / Minimum character count per line
22  ; 默认，当值为-1时则禁用自动文本格式化 / Default, setting to -1 disables auto text formatting

#Text-MaxLength ; 单行最大字数
24  ; 同上 / Same as above

#Before-Replaces ; 格式化前替换文本
[]:[] ; 格式： [原来文本]:[替换文本] 支持替换多个，使用换行区分
      ; Format: [original text]:[replacement text], supports replacing multiple items separated by newlines

#After-Replaces ; 格式化后替换文本
[]:[] ; 同上 / Same as above
```
如果想针对某一行不使用自动格式化，可以在前面加上`@::`，例如： <br>
If you want to skip automatic formatting for a specific line, add `@::` at the beginning, for example:
```
#0x1E4B
★◎  001  ◎★//@s現在の通常文字表示速度で表示しています。@s切り替えはオートに依存しています。
★◎  001  ◎★@::@s当前文字显示速度样本。（补丁仅供学习交流使\n@s用，严禁一切录播直播与商业行为）
```
***
默认情况下，mes的文本是以`sjis(cp932)`来解析。<br>
By default, mes text is parsed using sjis(cp932).
如需指定编码，需要exe的重命名加上`-cp[xxxx]`，例如：<br>
To specify a different encoding, rename the executable to include `-cp[xxxx]`, for example:

```
MesTextTool-cp936.exe ; cp936 -> GBK编码 / GBK encoding
```
程序默认是自动选择mes版本，**但不一定全都适用**，这时候就需要手动指定游戏版本了<br>
By default, the program automatically detects the mes version, **but it may not always work**. In such cases, you need to manually specify the game version<br>

将exe重命名加上`-版本简称`，例如：<br>
Rename the executable by adding `-version_alias`, for example:<br>
```
MesTextTool-cp936-dc4.exe ; 以GBK编码打开DC4版本的Mes文件 / Open DC4 version Mes files with GBK encoding
```
| 游戏 / Game | 简称 / Alias |
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
|ホームメイド -Home maid- 体験版| homemaidt |
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
|うたう絵本４（ADVTXT）|utaeho4|
|うたう絵本５|utaeho5|
|うたう絵本６|utaeho6|
|インファンタリア（ADVTXT）|infantaria|
|Aries -アリエス-（ADVTXT）|aries|
|D.C.体験版（ADVTXT）|dc|
|水夏ちょこっと体験版（ADVTXT）|suikademo|

上面那个列表上没有的，可通过命令行参数指定mes的info，数据结构参考：[advtxt_info](https://github.com/cokkeijigen/MesTextTool/blob/master/MesTextTool/src/mes/mes_advtxt.cpp#L16)、[script_info](https://github.com/cokkeijigen/MesTextTool/blob/master/MesTextTool/src/mes/script_info.cpp#L39) 。<br>
For games not listed above, you can specify mes info via command line arguments. For data structures, refer to: [advtxt_info](https://github.com/cokkeijigen/MesTextTool/blob/master/MesTextTool/src/mes/mes_advtxt.cpp#L16), [script_info](https://github.com/cokkeijigen/MesTextTool/blob/master/MesTextTool/src/mes/script_info.cpp#L39).
```
-mes=dc3wy,offset2,0xA09F,0x00,0x38,0x39,0x41,0x42,0x5F,0x60,0x63,0x64,0xFF,0x20,0x55
-advtxt=aries,0x00,0x0C
```
mes的opcode section获取参考 [Add Princess Party](https://github.com/cokkeijigen/MesTextTool/pull/5)，一般可以通过字符串`MESファイルを更新してください`快速定位。<br>
For obtaining mes opcode sections, refer to [Add Princess Party](https://github.com/cokkeijigen/MesTextTool/pull/5). Usually, it can be quickly located using the string `MESファイルを更新してください`.<br>

advtxt是根据指定op过滤出文本，默认是0x00，部分游戏的对话人名是单独的op，旧版的结构很简单，用winhex查看一下子就能找到了。<br>
advtxt filters text based on specified opcodes (default is 0x00). In some games, character names use separate opcodes. The structure of older versions is simple and can be easily found using WinHex.

## 0x3 如何编译 / How to Build（Windows）

- **前提准备 / Prerequisites ：**
**安装 [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) 和**
**[Cmake](https://cmake.org/download/)。**<br>
**Install [Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) and [Cmake](https://cmake.org/download/).**

- **使用 `git clone https://github.com/cokkeijigen/MesTextTool` 克隆仓库，**
**或者下载 [MesTextTool-master.zip](https://codeload.github.com/cokkeijigen/MesTextTool/zip/refs/heads/master) 并解压。**<br>
**Use `git clone https://github.com/cokkeijigen/MesTextTool` to clone the repository, or  [MesTextTool-master.zip](https://codeload.github.com/cokkeijigen/MesTextTool/zip/refs/heads/master) and extract it.**<br>

- **打开`cmd`或者`poweshell`, `cd`到源码路径，运行`build.bat`**<br>
**Open cmd or powershell, cd to the source code path, and run `build.bat`**
```sh
git clone https://github.com/cokkeijigen/MesTextTool
cd MesTextTool
build.bat
```
**运行完`build.bat`，输出exe的路径为源码目录下`.\build\Release\MesTextTool.exe`**<br>
**After running build.bat, the output executable path will be `.\build\Release\MesTextTool.exe` relative to the source directory.**
