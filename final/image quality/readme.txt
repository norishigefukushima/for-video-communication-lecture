使い方：
※もしVisual Studio 2015が入って以内場合、vcredist_x64.exeをインストールしてください．

YチャネルのPSNRが出力される．
psnr src.png dest.jpg 0

Yチャネルを出力する場合は数字を省略可能
psnr src.png dest.jpg 

-nオプションをつけると余計な情報が表示されない．
psnr src.png dest.jpg 0 -n 

数字の意味
０ Y
１ YUVの平均値
２ RGBの平均値
３ Y,U,Vの順で表示
４ R,G,Bの順で表示
