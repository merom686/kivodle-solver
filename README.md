[Kivodle](https://taktstockjp.github.io/Kivodle/)のソルバーです。  
4手先までを1秒程度で読み切ることができ、色々な機能が作れそうですが、現在のコードは最善の初手と手数を表示するのみとなっています。

開発環境がVisual C++なので次の行を入れてUTF-8に対応しています。
```cpp
setlocale(LC_CTYPE, "ja_JP.UTF-8");
```
Visual C++を使う場合は、コンパイルオプションに`/utf-8`を付けてください。

JSON形式のデータを用意する必要があります。
1. ブラウザでKivodleを開く
1. F12を押して開発者ツールを開きコンソールタブを選ぶ
1. `({ weapons, classes, schools, attackTypes, students })`を実行しオブジェクトをコピー
1. Kivodle.jsonとしてファイルに保存

何かあれば[@merom686](https://x.com/merom686)までお願いします（バグらせまくったので自信ない）。
