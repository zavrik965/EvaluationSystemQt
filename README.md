# СистемаЗачётов
# Установка
Для установки на систему с пакетной базой apt и пакетами deb необходимо:
```bash
wget -O - http://alexavr.ru/repo/keyFile | sudo apt-key add -
sudo echo 'deb [arch=amd64] https://alexavr.ru/repo/ focal contrib' > /etc/apt/sources.list.d/myrepos.list
```
Для установки на систему с пакетами rpm необходимо скачать послледнюю версию из вкладки _**Releases**_ и установить командой:
```bash
rpm -i EvaluationSystem-*.rpm
```