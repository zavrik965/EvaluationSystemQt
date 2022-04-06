# СистемаЗачётов
# Установка
Для установки на систему с пакетной базой apt и пакетами deb необходимо:
```bash
wget -O - https://alexavr.ru/repo/keyFile | sudo apt-key add -
sudo echo 'deb [arch=amd64] https://alexavr.ru/repo/ focal contrib' > /etc/apt/sources.list.d/myrepos.list
```
Также доступен вариант установки через **flatpak**:
- загрузить flatpak пакет из релиза
- выполнить команду: ```flatpak install EvaluationSystem.flatpak```

Для других систем необходимо собрать вручную из исходников

# Система подбора задач
Система подбора задач является дополением к Системе Зачётов
