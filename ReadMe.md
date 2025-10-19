# LoLLIPoP: Lots of Latency Letter IP over Post

物理的な郵便物を用いて IP データグラムをカプセル化し、既存の郵便システムを利用してエンドツーエンドの通信を実現する手段を提供します。

## これはなに？

LoLLIPoP は、郵便システムの通用する範囲を一つのデータリンクと見做し、封書やはがきなどの物理的な郵便物を用いて IP データグラムをエンドツーエンドで配送する、耐遅延性を有するネットワークプロトコルです。
LoLLIPoP を利用することで、有線接続や情報通信機器を利用できないような劣悪な環境下であっても、IP 等を用いた通信を実現できます。

## 環境

少なくとも次の環境で動作を確認しています。
TUN/TAP デバイスを利用できる必要があります。

```console
$ uname -a
Linux iyokan.local.kusaremkn.com 6.14.0-33-generic #33~24.04.1-Ubuntu SMP PREEMPT_DYNAMIC Fri Sep 19 17:02:30 UTC 2 x86_64 x86_64 x86_64 GNU/Linux
$ cat /etc/os-release 
PRETTY_NAME="Ubuntu 24.04.3 LTS"
NAME="Ubuntu"
VERSION_ID="24.04"
VERSION="24.04.3 LTS (Noble Numbat)"
VERSION_CODENAME=noble
ID=ubuntu
ID_LIKE=debian
HOME_URL="https://www.ubuntu.com/"
SUPPORT_URL="https://help.ubuntu.com/"
BUG_REPORT_URL="https://bugs.launchpad.net/ubuntu/"
PRIVACY_POLICY_URL="https://www.ubuntu.com/legal/terms-and-policies/privacy-policy"
UBUNTU_CODENAME=noble
LOGO=ubuntu-logo
$ modinfo tun
name:           tun
filename:       (builtin)
alias:          devname:net/tun
alias:          char-major-10-200
license:        GPL
file:           drivers/net/tun
author:         (C) 1999-2004 Max Krasnyansky <maxk@qualcomm.com>
description:    Universal TUN/TAP device driver
```

## コンパイル

`make` だけでコンパイルできます。
実行ファイル `lollipop` が出現します。

```console
$ make
cc -o lollipop main.c
```

## 使い方

次のように実行します。
ただし、送信されたパケットを一時保存するためのディレクトリを予め用意しておく必要があります。

```console
# lollipop [-i devname] [-s sockpath] [-S spooldir]
```

ここで、各パラメータには以下を指定します。

- *devname*: ネットワークインタフェース名を指定します。
    文字列 **%d** を末尾に含むべきです。
    デフォルトでは **post%d** が指定されます。
- *sockpath*: パケットの受信に用いる UNIX ドメインソケットの名前を指定します。
    デフォルトでは **/var/run/lollipop.socket** が指定されます。
    プログラムの開始時に unlink(2) されます。
- *spooldir*: 送信されたパケットを一時保存するためのディレクトリの名前を指定します。
    デフォルトでは **/var/spool/lollipop/** が指定されます。
    既存のディレクトリを指定する必要があります。

プログラムが実行されるとネットワークインタフェースが追加されます。
例えば、次のように実行してネットワークインタフェースを有効にし、アドレスを割り当てます。
ただし、ネットワークインタフェース名 *post0* や IPv4 アドレス *192.0.2.57/24* は適当に置き換えられる必要があります。

```console
# ip link set post0 up
# ip address add dev post0 192.0.2.57/24
```

## パケットの送受信

### パケットの送信

ネットワークインタフェースから送信されたパケットは、送信されたパケットを一時保存するためのディレクトリにファオルとして保存されます。
ファイル名は `"%s.%d.%d.%06d"` のように命名され、各フィールドは左から順に、ネットワークインタフェース名、通し番号、紀元からの秒数、マイクロ秒に対応します。
各ファイルには、生の L3 パケットがそのままの形式で保存されます。

フロントエンドプログラムは、これらのファイルを適切な形式に変換し、物理的な郵便物として出力する必要があります。

### パケットの受信

ネットワークインタフェースが受信するパケットは、パケットの受信に用いる UNIX ドメインソケットにデータグラムとして送信される必要があります。
各データグラムは、生の L3 パケットをそのままの形式でもっている必要があります。

フロントエンドプログラムは、物理的な郵便物に記録されているパケットを適切に変換し、このソケットに送信する必要があります。

## スペシャルサンクス

- SHINTRANET-Lab [信濃 眞伊](https://github.com/Mai-Shinano): 発案者
- SCRWNLNET [佐伯 真紘](https://github.com/scrwnl): 命名者
