# LoLLIPoP: Lots of Latency Letter IP over Post

物理的な郵便物を用いて IP データグラムをカプセル化し、既存の郵便システムを利用してエンドツーエンドの通信を実現する手段を提供します。

> [!CAUTION]
> このプログラムにはセキュリティ上の問題があります。
> 正しくない使い方をすると、意図しないファイルを破壊したり、システムを危険に晒すことにつながります。

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

コマンド `make` を実行すると一連のプログラムがビルドされます。
ディレクトリ **lollipopd** 及び **lollipop** 配下の各ディレクトリに実行ファイルが出現します。

```console
$ make
```

## インストール

まず、以下のディレクトリを作成する必要があります。

- /var/spool/lollipop/waiting/
- /var/spool/lollipop/sending/
- /var/spool/lollipop/sent/
- /var/spool/lollipop/removed/

その上で、各種実行ファイルを適切なパス（例えば、**/usr/local/sbin/**）にインストールします。

## 使い方

まず、**lollipopd** を起動して仮想ネットワークインタフェースを生成します。
オプション **-i** を利用してインタフェース名を指定できます（デフォルトでは *post0* です）。

> [!NOTE]
> **lollipopd** はまるで daemon のような名前をしていますが、今のところ daemonise しません。
> この挙動は後のバージョンで修正されます。

生成されたネットワークインタフェースを有効にします。
現代的な Linux 環境であれば、次のように実行します。
また、DHCP を利用しないのであれば、静的にアドレスを割当てることもできます。

```console
# ip link set post0 up
# ip address add dev post0 192.0.2.57/24
```

## パケットの送受信

### パケットの入力（受信）

パケットを入力するには、**lollipop-in** を利用します。
**lollipop-in** は標準入力からパケットデータを読み込み、そのネットワークインタフェースから入力します。
オプション **-i** を利用してインタフェース名を指定できます（デフォルトでは *post0* です）。

入力されるパケットは次の形式のバイナリである必要があります。

1. 16 bit のフラグ情報
2. 16 bit のプロトコル識別子
3. L3 データグラムの本体

郵便物に base64 でエンコードされたパケットデータが記載されている場合、次のように実行してパケットを入力できます。

```console
# base64 -d | lollipop-in
```

### パケットの出力（送信）

生成されたパケットがあれば、**lollipop-ls** で確認できます。
**lollipop-ls** はパケットの識別子の一覧を表示します。

> [!NOTE]
> **lollipop-ls** は手抜きで作られています。
> **.** や **..** と名付けられた項目が見えますが、これは実際にはパケットではありません。
> この挙動は後のバージョンで修正されます。

生成されたパケットを出力するには、**lollipop-out** を利用します。
**lollipop-out** は引数で指定された識別子をもつパケットを標準出力に書き出します。

郵便物に印刷するためにパケットを base64 にエンコードする場合、次のようにしてパケットを出力できます。

```console
# lollipop-out hogehoge | base64
```

## スペシャルサンクス

- SHINTRANET-Lab [信濃 眞伊](https://github.com/Mai-Shinano): 発案者
- SCRWNLNET [佐伯 真紘](https://github.com/scrwnl): 命名者
