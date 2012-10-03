## 特性

  * 支持拼音首字母匹配和完全匹配
  * 支持多音字匹配

## 安装
### ubuntu
支持10.04,10.10,11.04,11.10,12.04版本从ppa安装

    sudo add-apt-repository ppa:emptyhua/toolbox
    sudo apt-get update
    sudo apt-get install bash-pinyin-completion

然后新开一个终端，就可以使用拼音补齐了

### Other Linux
对于其他发行版本，只要安装了bash－completion都可以使用源码安装(已测的bash－completion最低版本为1.1)

    git clone git://github.com/emptyhua/bash-pinyin-completion.git
    cd bash-pinyin*
    make
    sudo make install
    
然后新开一个终端，就可以使用拼音补齐了

### Mac OS X
对于mac需要先安装 bash-completion

    sudo port install bash-completion

然后编辑~/.bash_profile,把下面的代码贴进去

    if [ -f /opt/local/etc/bash_completion ]; then
    . /opt/local/etc/bash_completion
    fi

下载源码,并编译,编译之前根据自己的xcode安装目录修改Makefile中的XCODE_SDK变量

    git clone git://github.com/emptyhua/bash-pinyin-completion.git
    cd bash-pinyin*
    make
    sudo make install

然后新开一个终端，就可以使用拼音补齐了

