Complete boundle of BibleTime Mini application.

Run update-all.sh to get it up to date.

Use Qt Creator and corresponding to target platform \*.pro file: ./bibletime/platforms/**platform**/btmini/btmini.pro

## Layout

Some notes that should help to manage with repository updates:

* `bibletime`: was added with
  ```
  git subtree add --prefix bibletime https://github.com/bibletime/bibletime.git v3.0.2
  ```
* `clucene`: was copied from Qt 4.8(?) source distribution, `<qt_sources_root>/src/3rdparty/clucene`, to provide cross platform compatibility.
* `zlib`: was added with
  ```
  git subtree add --prefix zlib https://github.com/madler/zlib.git master
  ```
