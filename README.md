# iqs550trackpad
iqs550trackpad test

* 僅限qmk 0.23.9 ，PXM0002測試用途，TPS65則是qmk原本就有內建。
  
  make binepad/bn003:default

* PXM0012 就是市面上有在賣的 TPS65，尺寸為65x49，但因為庫存，所以firmware版本是A000的，

  可以用iqs550ino_A000\的arduino程式操作，datasheet要看A000版的，

  也可以用官方給的Arduino程式刷成B000，這個程式的預設hex就是給TPS65使用的
      
* PXM0002 則是市面上找不到的，尺寸為75x47，原先的firmware版本不確定，總之刷成B000了，

  因為官方給的hex是給TPS65的，沒有官方工具燒錄檔不好改，

  所以如果要正常使用的話需要用軟體做初始設定，

  和TPS65主要是TX channel不同，PXM0002是0~15順序排列，TX全部打開。

  因為靈敏度太強，把ATI Target設350，ATI C設2即可正常使用。

  因為沒有官方工具，沒有辦法進行微調，但已堪用。

* iqs550ino_A000\     for firmware A000
  其它都是官方網站給的
 
