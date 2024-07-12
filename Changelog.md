# Mod Manager Changelog

## 正式发布 Release

### v1.1.1

*2024.07.12*

Update to the lateset API(Curseforge/Modrinth). Thanks to @Forgot-Dream .
更新了最新的网站API（Curseforge/Modrinth），感谢：@Forgot-Dream 

Added speed chart display for downloader.
添加了下载器的速度图表绘制。

Web Images in mod descriptions can be displayed now.
mod描述中在线图片也能显示了。

Added more frameless windows.
添加了更多窗口的无边框显示。

Tweaked windows title bar.
调整了标题栏样式。

Tweaked QSS Stylesheet.
调整了QSS样式表。

Changed  default download path for self-update.
更改了Mod管理器自我更新的下载位置。

Improved performance for list widget.
优化了列表窗体的性能。

Fixed display issues when open a subwindow under frameless mode.
修复了无边框窗口模式下打开子窗口时出现的问题。

Fixed menubar under native window.
修复了原生窗口下的菜单栏问题。

### v1.1.0

*2022.03.02*

Stylesheets can be customized under app config directory.
样式表可以在应用配置目录下自定义。

Changed shortcut of prefereneces to `ctrl + ,` .
将偏好设置的快捷键更改为了`ctrl + ,` 。

Tweaks on frameless UI:
无边框界面上的修改：

- Window title is at center now.
  窗口标题现在是居中的了。
- Fixed window resize handler.
  修复了窗口缩放的控制。
- Added system menu on app icon.
  在应用图标上添加了系统菜单。

### v1.0.5

*2022.02.18*

Mod Manager is auto released after new commit.
Mod管理器会在新的提交后自动发布版本。

Fixed context menu in other view mode in explore pages.
修复了浏览页面中其他显示模式中的右键菜单。

Added button to open curseforge/modrinth website in info widget.
在信息窗体上添加了按钮来打开Curseforge/Modrinth的网页。

Languages setting is in preferences now.
语言设置现在在偏好设置里了。

Releases in GitHub pages has coressponding height to its content.
GitHub页面中的发布拥有与内容一致的高度了。

Tweaked stylesheets.
调整了样式表。

Tweaked status bar in download page.
调整了下载页面中的状态栏。

Fixed download button in modrinth page.
修复了Modrinth页面中的下载按钮。

### v1.0.4

*2022.02.05*

Added icon/detail mode for explore mod pages, as local mod pages.
为浏览mod页面添加了图标与细节模式，与本地mod页面保持一致。

Added cache for Modrinth icons.
为Modrinth的图标添加了缓存。

Fixed title bar may uncolsed when close window in frameless UI.
修复了无边框界面中标题栏没有随窗口关闭的问题。

Fixed 'Max/Min Window' button in frameless UI.
修复了无边框界面中的“最大化/最小化窗口”的按钮。

Fixed title bar blur offset when maximumized in frameless UI.
修复了无边框界面中最大化时标题栏背景模糊的偏移。

Fixed crash on about dialog in frameless UI.
修复了无边框界面中关于对话框的崩溃。

Fixed potential crash under KF5.
修复了KF5下潜在的崩溃。

### v1.0.3

*2022.01.28*

Tweaked status bar, moved mod count to right side.
调整了状态栏，将mod计数移动至右侧。

Tweaked tool bar and search bar in explore page.
调整了浏览页面中的工具栏和搜索栏。

Tweaked add/remove tag actions in local mod pages, it can operate on multi-mods now.
调整了添加/移除标签

Added more actions to downloader.
为下载器添加了更多操作。

Fixed crash caused by actions on refreshed mod in explore pages.
修复了由在浏览页面中已刷新的mod上进行操作产生的崩溃。

Fixed mod link stuck.
修复了mod链接卡住。

### v1.0.2

*2022.01.24*

Added update check for Mod Manager itself.
添加了对Mod管理器自身的更新检查。

Download item will show in download list immediately when added now. Tweaked actions in download pages.
下载任务现在在添加后会立即显示在下载列表中。调整了下载页面中的一些操作。

Tweaked context menu in local mod pages.
调整了本地mod页面中的上下文菜单。

Fixed menu bar in non-frameless UI.
修复了非无边框界面中的菜单栏。

Fixed tags filter in local mod pages.
修复了本地mod页面中标签筛选的问题。

### v1.0.1

*2022.01.22*

Fixed Curseforge Page crash.
修复了Curseforge页面中的崩溃。

Fixed menu bar in framess UI.
修复了无边框界面中的菜单栏。

### v1.0.0

*2022.01.22*

Used MDI mainwindow layout for better subpage management, and added much more menu actions for each page. Changed buttons in pages to tool bar to make it more flexible.
使用MDI主界面布局，更利于分页管理，为各个页面添加了更多的菜单操作。将页面中的按钮改为使用工具栏的形式，使界面更加灵活。

Added standable categories/ game versions tool bars for Curseforge/Modrinth.
为 Curseforge/Modrinth 添加了独立的类别、游戏版本工具栏。

Added GitHub page, you can view and download GitHub Releases of modmanager, or open releases page in local mod page.
添加了GitHub页面，可以浏览并下载GitHub Releases，也可以直接从本地mod中直接打开Releases页面。

Curseforge page now can not only explore mod but also modpacks, resourcepacks and worlds.
Curseforge页面现在除了Mod之外还支持查看并下载整合包、资源包和存档。

Added support for local mod exporting some useful files for modpacks: `manifest.json`, `modlist.html`or compressed file.
本地Mod管理支持导出一些整合包所用的常用文件：`manifest.json`、`modlist.html`或压缩包。

Added support for import and download Curseforge modpacks.
支持导入并下载Curseforge整合包。

Added supprt to embedded aria2 downloader for custom download.
内嵌的aria2下载器支持自定义下载。

Added sort action in context menu of local mod list.
在本地mod列表的右键菜单中添加了排序的选项。

Enabled OptiFine download from BMCLAPI.
启用了使用BMCLAPI渠道下载OptiFIne。

Added support to proxy settings.
支持代理设置。

Added lanaguage tags, added automatically by lang file in mod.
添加了语言标签，会根据mod内的语言文件自动添加。

Link file now linked all mod files, including old files.
链接文件会自动链接所有mod文件（包含旧版文件）。

Tweaked date time format to make it more readable.
修改了日期时间格式，提升可读性。

Tweaked install directory under windows: x64-arch modmanager will be installed under Program Files not Program Files (x86). 
调整Windows下的安装路径，64位会安装在 Program Files 下而不是 Program Files (x86) 下。

Tweaked local mod filter, it has more filter options now.
调整了本地mod筛选，拥有更多的筛选选项。

Tweaked local mod paths manager dialog.
调整了本地mod路径管理器对话框。

Tweaked mouse scrolling of tags.
调整了鼠标滚动在标签上的滚动。

Tweaked Frameless UI for Windows.
调整了Windows下无边框界面的观感。

Curseforge categories is fetched dynamically, you may need to wait icon loading when firstly use.
将Curseforge的分类调整为动态获取，初次打开可能需要等待图标加载。

Tweaked UI and stylesheet.
调整了UI，调整了样式表。

Reduced height of status bar.
减小了状态栏的高度。

Update check failure will show failed mod.
更新检查失败会显示失败的mod。

Fixed update check stuck (?).
修复了更新检查卡住（？）。

Fixed cancel update checking.
修复了取消检查更新。

Fixed search issue of Modrinth.
修复了Modrinth搜索的问题。

Fixed lag when opening Curseforge/Modrinth dialog.
修复了打开 Curseforge/Modrinth 对话框时出现的卡顿。

Fixed icon blurred uder HiDPI.
修复了HiDPI下的图标模糊。

Fixed update checking not check Alpha version.
修复了更新检查不检查Alpha版本的问题。

Fixed some forge mods not loaded.
修复了一些forge mod没有被加载。

Fixed issue of Minecraft formatted code, added more support for formatted code.
修复了MC样式文本的问题，为样式文本添加更多的支持。

Fixed count error in Explore pages.
修复了“探索”页面中的计数错误。

Fixed some issues of local mod path auto naming.
修复了mod路径自动命名的一些问题。

Fixed an untranslated text.
修复了一处未翻译的文本。

## 公开测试 Beta Test

### v1.0.0-beta.8

*2021.12.12*

Added lazy loading of local mods and explore page, improving startup proformance.
添加了本地mods和浏览页面缓式加载，提升启动时的性能。

Added local mod path info display.
添加了本地mod路径信息显示。

Added multi-mod info display when select multiple local mods.
在多选本地mod时，添加了多个mod信息显示。 

Added standardalone dialog for explore pages.
为探索页面添加了独立对话框界面。

Added corresponding actions in menubar for different pages.
为不同页面在菜单栏中添加了对应的操作。

Added more mod information in table view. You can select which item to be displayed.
向表格视图中添加了更多的mod信息。你可以选择那些项目被显示。

Added batch rename history and fast batch rename based on rename history.
添加了批量重命名历史和基于批量重命名历史的快速批量从重命名

Added multi-section mod actions to right click menu on local mod.
为本地mod的右键菜单添加了多项选择的mod操作。

Added manual mod search for unlinked mod-ids.
为未链接的mod-id添加了手动mod搜索。

Added Curseforge/Modrinth/OptiFine tags for local mod.
为本地mod添加了Curseforge/Modrinth/OptiFine标签。

Added status bar for explore pages.
为探索页面添加了状态栏。

Added loader type for Modrinth mods display.
添加了Modrinth mod的加载器版本显示。

Added OptiFine version detection.
添加了OptiFine版本检测。

Update check will show all available updates(not latest).
更新检查会显示所有可用的更新（非最新的）。

Tweaked dock widgets to make them more flexible and show title when unlocked.
调整了停靠部件使其更灵活，并在未锁定时显示标题。

Modrinth file list auto fetch is toggleable, off by default.
自动获取Modrinth文件列表是可选择的，默认为关。

Fixed(?) update checking stucked.
修复了（？）更新检查卡住。

Fixed formatted name in icon/table view and in disabled state.
修复了样式代码名称在图标/表格视图以及禁用状态中的显示。

Fixed local mod file links.
修复了本地mod文件链接。

Fixed local modid cache.
修复了本地modid缓存。

Fixed local mod icon scale aspect.
修复了本地mod图标缩放宽高比。

### v1.0.0-beta.7

*2021.11.27*

Added icon view and table view for local mod list.
为本地mod列表添加了图标和表格视图。

Added local mod file list, you can switch mod version from here.
添加了本地mod文件列表，你可以在这里切换mod版本。

Added support to sub-directories.
添加了对子文件夹的支持。

Added sub-directory tags and file name tags. They are auto generated by path name and file name.
添加了子文件夹标签和文件名称标签，它们是根据路径名和文件名自动生成的。

Added two patterns for batch rename:
为批量重命名添加了两种格式：

- `<capture|...|...|...>`

  *1st arguement* is the text to be replaced; *2nd arguement* is the reg-exp to capture strings in file names; *3rd argument* is the text where uses the captured strings.

  For example: Using `<capture|<filename>|\[(.+?)\]|(\1)>` while file name is `[1.15+][tool]modid-version.jar` will result in `(1.15+)(tools)` , capturing and replacing all square brackets with braces.

  *第一个参数* 为需要被捕获的文本；*第二个参数* 为在文件名中捕获字符串的正则表达式；*第三个参数* 为使用捕获字符串的文本。

  例如：文件名称为`[1.15+][tool]modid-version.jar`时，`<capture|<filename>|\[(.+?)\]|(\1)>`会获得`(1.15+)(tools)`的结果，捕获并将所有的方括号替换为圆括号。

- `<replace|...|...|...>`

  *1st arguement* is the text to be replaced; *2nd argument* is reg-exp to search/match in *1st arguement* ; *3rd argument* is the string to replace the text.

  For example: Using `<replace|<filename>|\[(.+?)\]|(\1)>` while file name is `[1.15+][tool]modid-version.jar` will result in `(1.15+)(tools)modid-version.jar` , replacing all square brackets with braces.

  *第一个参数* 为需要被替换的文本；*第二个参数* 为用于搜索/匹配的正则表达式；*第三个参数* 为用于替换文本的字符串。

  例如：文件名称为`[1.15+][tool]modid-version.jar`时，`<replace|<filename>|\[(.+?)\]|(\1)>`会获得`(1.15+)(tools)modid-version.jar`的结果，将文件名中所有的方括号替换为圆括号。

Added auto completer for rename patterns.
为重命名格式添加了自动补全。 

Added mod file release type (release/beta/alpha) display. You can choose update source of release type.
添加了mod文件发布类型（正式版/测试版/不稳定测试版）显示。你可以选择更新检查时是否使用该发布类型。

Added online translator (Youdao Translator) for mod summaries.
添加了有道生草机在线翻译mod简介。

Updated Curseforge's categories.
更新了Curseforge的分类。

Tweaked style of tables in update/batch-rename dialog. Tweaked combo box item in update dialog.
调整了更新与批量重命名对话框中的表格样式。调整了更新对话框中的下拉框项目。

Tweaked styles of Curseforge/Modrinth tags.
调整了Curseforge/Modrinth标签的样式。

Mod file release type and game version are togglable.
mod发布类型和游戏版本是可勾选的。

Improved performance while loading file list in Curseforge/Modrinth.
优化了加载Curseforge/Modrinth文件列表时的性能。

Using system icon theme is configurable now.
使用系统图标主题是可配置的了。

Fixed add new path button not working.
修复了添加新路径按钮不工作的问题。

Fixed progress bar in local mod page disappeared in some circumstances.
修复了本地mod页面中进度条在一些情况下消失的问题。

Fixed some issues caused by reloading local mods.
修复了重新加载本地mod时出现的一些问题。

Fixed unnecesssary ui updates when not accept preference changes.
修复了未接受偏好设置变更时产生的不必要更新。

Fixed(?) update checking stucked. Added retwork request timeout setting.
修复了（？）更新检查卡住。添加了网络请求超时设置项。

Removed combo box for local mod sorting , because you can sort it simply by clicking on table's header.
移除了本地mod排序的下拉框，因为现在只需要点击表格的表头就可以排序了 。

Removed period trim of mod description.
移除了mod描述在句号处的裁剪。

### v1.0.0-beta.6

*2021.11.14*

Added mod info and file list side bar and tweaked "Page Selector" sidebar, they are all configurable.
添加了mod信息和文件列表侧边栏并调整了“页面选择器”侧边栏，它们都是可自由配置的。

Added "Gallery" in Curseforge Mod Dialog to show image info of mods.
在Curseforge mod对话框中添加了“画廊”来显示mod图片信息。

Added ui style(theme) switch.
添加了UI风格（主题）切换。

Added BlurBebind effects for windows to enhanced visual effects (optional).
在Windows下添加了背景模糊的效果来加强视觉效果（可选的）。

Added frameless ui for windows (optional).
在Windows下添加了无边框界面（可选的）。

Fixed snapshot version issue in Modrinth page.
修复了Modrinth页面中快照版本显示。

Fixed opening external link in mod description.
修复了打开mod描述中外部链接的问。

Fixed "Page Selector" selected item unsynced from main ui.
修复了“页面选择器”选中项与主界面不同步。

### v1.0.0-beta.5

*2021.10.30*

Rewrited entire downloader with Aria2, now it's faster and more stable.
使用Aria2重写了整个下载器，现在更加稳定。

Added more mod info display: updated/created time, loader types, they are all togglable.
添加了更多mod信息显示：更新创建时间、加载器版本，它们都是可选择显示的。

Added smooth scrolling to mod description.
为mod描述添加了平滑滚动。

Added more actions in menu bars, along with shortcuts.
向菜单栏中添加了更多操作，以及快捷键。

More advanced version/category selection and multi-selection in modrinth.
更高级的版本和类别选择，支持Modrinth的多项选择。

All "explore" pages are togglable in prefercences.
所有“探索”页面在偏好设置中都是可选择显示的。 

Fixed crashes in most cases. It's much more stable now.
修复了多数情况下的崩溃。现在更加稳定了。

Fixed Curseforge/Modrinth dialog cannot be reopened after closed.
修复了Curseforge/Modrinth对话框在关闭后无法重新打开的问题。

Fixed update failed when mod file is universal for fabric and forge.
修复了mod文件是fabric/forge通用时会更新失败的问题。

### v1.0.0-beta.4

*2021.10.20*

Added Linux/MacOS app packages.
添加了Linux和MacOS的打包。

Added mod file website infomation cache, website searches in startup got faster. Mod version switch will recheck update now.
添加了mod文件网站信息缓存，启动时的网站搜索变得更快了。Mod版本切换后会重新检查更新。

Add mod path loader type auto determination.
添加了mod路径自动判断加载器类型。

Added multiple path imports.
添加了多路径批量导入。

Added environmantal client/server tag.
添加客户端或服务端的环境标签。

Added option to ignore mod update, you can ignore certain versions not suitable for this path.
添加了忽略mod更新的选项，你可以忽略一些不适合当前路径的版本。

Added support to color code format of local mod name, author name and description.
添加了对本地mod的名称、作者名字和描述的颜色代码格式的支持。

Attempted to optimize memory usage, may introduce some **crashes**.
试图优化了一些内存占用，也许会引入一些**崩溃**。

Standardized data storage paths.
规范化软件数据存储路径。

OptiFine will show in Fabric mod path as well.
OptiFine也会在Fabric路径中显示了。

Tweaked smooth scrolling, and it's configurable now.
调整了平滑滚动，而且现在是可配置的。

Tweaked Local mod list pages. They have a status bar to show info and progresses.
调整了本地mod列表页面，使用状态栏来显示信息和进度条。

Tweaked update dialog, hovering on version name will show some details.
调整了更新对话框，鼠标悬停在版本名称上会显示一些详细信息。

Fixed some issue in Curseforge/Modrinth searches.
修复了一些Curseforge/Modrinth搜索上的问题。

Fixed local mod search returns nothing.
修复了本地mod搜索没有任何结果。

Fixed some forge mod load failures.
修复了一些Forge mod加载失败的问题。

Fixed website button unclickable in modrinth mod dialog.
修复了Modrinth中无法点击的网站按钮。

Other random tweaks and bugfixes.
以及一些其他的小调整和bug修复。

### v1.0.0-beta.3

*2021.10.10*

Added local mod filters.
添加了本地mod筛选。

Added caches for mod icon to load faster.
添加了图标缓存，提升加载速度。

Added Curseforge/Modrinth categories display.
添加了Curseforge/Modrinth的类别显示。

Added local mod loading progress display.
添加了本地mod加载进度显示。

Mod lists now have smooth scrolling.
mod列表现在有平滑滚动了。

Resize/reposition of main window will be saved.
主界面的调整大小和位置会被保存。

Local mods without icon info will show website icons.
没有图标的本地mod会显示存在的网络图标。

Browser selector is movable toolbar now.
浏览选择器是可移动的工具栏了。

Corrected search sort of Curseforge. It differed from website's in fact.
纠正了Curseforge的搜索排序，它实际上与网站上的排序不同。

Tweaked Curseforge / Forge icon.
调整了Curseforge和Forge的图标。

Tweaked GUI of mod list.
调整了mod列表的GUI。

Mod summaries of Curseforge and Modrinth are not in italic font.
Curseforge和Modrinth的mod概述不以斜体显示了。

Fixed browser selector jump to downloader tab.
修复了浏览选择器会跳到下载器标签的问题。

### v1.0.0-beta.2

*2021.10.03*

Added **tags** system for local mod management.
为本地mod管理添加了**标签** 系统。 

Added **Batch Rename** feature, you can customize your mod file name by tags.
添加了**批量重命名** 功能，现在可以使用标签来个性化你的mod文件名称了。

Added Traditional Chinese (zh_TW) translation.
添加了繁体中文（zh_TW）的翻译。

Some GUI tweaks.
一些GUI上的小修改。

Fixed Modrinth mod file not presented as "Downloaded" in local mod path tab.
修复了本地mod浏览的Modrinth对话框中不显示“已下载”的问题。

### v1.0.0-beta.1

*2021.09.30*

First public version. Have a nice day!
第一个公开测试版。希望大家用的开心。

## 内部测试 Alpha Test

Details are not listed.
更新内容不列出。

### v1.0.0-alpha.8

*2021.09.29*

### v1.0.0-alpha.7

*2021.09.24*

### v1.0.0-alpha.6

*2021.09.21*

### v1.0.0-alpha.5

*2021.09.17*

### v1.0.0-alpha.4

*2021.09.13*

### v1.0.0-alpha.3

*2021.09.11*

### v1.0.0-alpha.2

*2021.09.10*

### v1.0.0-alpha.1

*2021.09.10*
