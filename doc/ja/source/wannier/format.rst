ファイルフォーマット
====================


HPhi/mVMCのスタンダードモードでは以下のファイルを読み込む.
これらのファイルはRESPACKのプログラムを実行すると, 
``dir-model`` ディレクトリに ``[CDataFileHead]=zvo`` として自動出力される.

ジオメトリー
------------

ファイル名は ``[CDataFileHead]_geom.dat``.
このファイルを必要に応じて修正することで取り扱う軌道数を変更できる.

::

   -1.917800 1.917800 6.280100
   1.917800 -1.917800 6.280100
   1.917800 1.917800 -6.280100
   3
   0.000000 -0.000000 -0.000000
   -0.000000 -0.000000 -0.000000
   0.000000 0.000000 0.000000

* 1 - 3行目

  結晶並進ベクトル. デカルト座標系で任意単位.

* 4行目

  mVMC/HPhiで取り入れる, ユニットセルあたりの軌道数.
  Wannier関数の数よりも少なくすることも可能であり,
  その場合には先頭からこの数の分だけの軌道を取り入れたモデルが構築される.

* 5行目以降

  各軌道のWannier中心 (フラクショナル座標). Fourier変換ツールで使われる.
  なお, ここで定義されたWannier関数の順番が, 以下の4つのファイルのWannier関数のインデックスに対応する.

ホッピング, Coulomb積分, 交換積分, 電荷密度
-------------------------------------------

ファイル名は ``[CDataFileHead]_hr.dat``, ``[CDataFileHead]_ur.dat``, 
``[CDataFileHead]_jr.dat``,  ``[CDataFileHead]_dr.dat``.
Wannier90のホッピング積分の形式に従う(詳細はwannier90のuser_guideの ``8.19 seedname_hr.dat`` を参照のこと).

::

  wannier90 format for mvmcdry
  8
  343
  1    1    1    1    1    1    1    1    1    1    1    1    1
  ...
   -3   -3   -3    1    1   0.0004104251  -0.0000000000
   -3   -3   -3    1    2   0.0001515941  -0.0000000006
   -3   -3   -3    1    3  -0.0001515941   0.0000000002

* 1行目

  ファイルヘッダ

* 2行目

  Wannier関数の数.

* 3行目

  スーパーセルの数 ``nrpts`` .

* 4行- 5 + int(``nrpts``/15) 行

  各スーパーセルでの縮退値 (基本的には1).

* 6 + int(``nrpts``/15) 行 -

  1-3列がスーパーセルの格子ベクトル,
  4列目が原点のWannier軌道のインデックス,
  5列目がスーパーセルのWannier軌道のインデックス,
  6列目が実数値, 7列目が虚数値をそれぞれ与える.
