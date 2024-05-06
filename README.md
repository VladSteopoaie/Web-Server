# Web-Server

This is a simple unix web-server designed to help me learn more about the how web servers and clients work.

## Some properties

- accepts GET, POST and HEAD requests
- can process simple php files
- is multi-threaded (the php processing is concurrent)

## Installation

#### Prerequisites

- gcc installed on the machine ([Here](https://ioflood.com/blog/install-gcc-command-linux/) is a guide)
- root privileges

Open a terminal in the desired directory on installation and proceed with the following commands:

```bash
git clone https://github.com/VladSteopoaie/Web-Server
cd Web-Server/webserver
make # should have gcc installed for this one
sudo mkdir -p /etc/webserver/http/home
sudo mv src/preprocess.php /etc/webserver
```
You can now move the web files into /etc/webserver/http/home and start the server with the command:

```bash
bin/webserver
```

If you want to make the programm grobally accessible follow this command:

```bash
sudo mv bin/webserver /usr/bin
```
Now you can type:

```bash
webserver
```
If you want to change some configurations of the web-server (root directory, port, number of threads):

```bash
sudo nano /etc/webserver/webserver.conf
```

And here is an example of configuration file:

```
root=/desired/path/to/the/web/files
port=8080
threads=8
```

Enjory playing and breaking it!
