ひとりWiki PlantUML プラグイン
====


これは何？
----

ひとりWikiでPlantUMLを使用するためのプラグインDLLです。

ただし、プラグインに複数行の引数を引き渡せるWikiスタイルにしないと使用できません。  
現在のところ、プラグインに複数行の引数を引き渡せるのは PukiWiki風 だけのようです。

※ひとりWikiとは、MASさんが作成されているフリーウェアの事です。
[https://mas3lab.net/soft/#htwiki](https://mas3lab.net/soft/#htwiki)


必要なもの
----

* ひとりWiki本体
* PlantUML(webモードで使用する場合は不要)

PlantUMLを動かすためには Java が必要です。また、クラス図などを描かせるためには Graphviz も必要です。  
ただし、[webモード] という使用方法で使われる場合は不要です。詳細は HHhtwikiPlugin\_PlantUML.config ファイル内のコメントをご覧ください。

PlantUML のインストール方法については PlantUML のホームページをご覧ください。
[https://plantuml.com/starting](https://plantuml.com/starting)


インストール方法
----

* ひとりWikiがインストールされているディレクトリ内にある plugin サブディレクトリに、HHhtwikiPlugin\_PlantUML.dll と HHhtwikiPlugin\_PlantUML.config を格納してください。PlantUML を自身のPC上で動かす場合は plantuml.jar も plugin サブディレクトリに格納してください。

* HHhtwikiPlugin\_PlantUML.config をテキストエディタで開き、ファイル内のコメントを参考に設定を編集してください。

* ひとりWiki(htwiki.exe)を起動してください。

* ひとりWikiから「プラグインの使い方」を開いてください。メニューの [ヘルプ(H)] ⇒[プラグイン(P)] を選択する事で開くことができます。

* 「プラグインの使い方」に HHhtwikiPlugin\_PlantUML.dll が表示されていればインストール成功です。


使い方
----

* Wikiの表示スタイルを PukiWiki風 に切り替えてください。

* 新しいページを作成してください。

* 新しいページを以下のように編集すると、PlantUML によってシーケンス図が描かれるハズです。もしエラーメッセージが表示された場合は HHhtwikiPlugin\_PlantUML.config の内容を再確認してください。設定の変更後は、ひとりWikiを再起動してください。

```
#PlantUML(){{
@startuml
Bob -> Alice : hello
@enduml
}}
```

制限事項
----

プラグインに複数行の引数を引き渡せるWikiスタイルにしなければ使用できません。  
現在のところ、プラグインに複数行の引数を引き渡せるのは PukiWiki風 だけのようです。
