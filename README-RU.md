## QUnSocket

Универсальный класс-обертка, позволяет установливать соединение и прием/передачу данных
по протоколам: `UNIX-sockets`, `HTTP`/`HTTPS`, `TCP`/`UDP`, `Websockets`.

* Весь код класса `QUnSocket` реализован в одном небольшом заголовочном `c++` файле
* Для создания соединения с сервером, по любому протоколу кроме `UNIX-sockets`, 
  методу `connectToServer()` передается строка в формате: 
  `protocol://location:port`
  где, атрибут `port` не указывается если используется стандарный порт (80 / 443).
  Для соединения по протоколу `UNIX-sockets` передается строка в формате:
  `unix:/location`
* Все протоколы имеют `бинарный канал` приема/передачи данных
* Протокол WebSockets имеет дополнительный `текстовый канал` приема/передачи данных
* Действия для создания соединения, приема/передачи данных и обработки ошибок 
  для всех протоколов `однообразны`.
* Все протоколы имеют возможность приема данных тремя способами:
  1. Синхронный (всегда буферизован)
  2. Асинхронный буферизованный
  3. Асинхронный не буферизованный
* Протокол `WebSockets` в дополнение к описанным выше методам приема бинарных данных, 
  имеет также методы для приема текстовых данных
* Обязательной для включения в проект является только библиотека `networks` (`QT` += `network`).
  Если в проекте не будут использоваться сокеты типа `WebSockets` то библиотеку `websockets`
  включать в проект не нужно.
* Совместим `Qt5`/`Qt6`

#### Паттерны способов приема данных:
```
         +-------------+----------------------------+
         |     SYNC    |           ASYNC            |
         +-------------+-------------+--------------+
         |          Buffered         | non-Buffered |
+--------+-------------+-------------+--------------+
|  All   |             |             |              |
|sockets |      S      |      AB     |     ANB      |
+--------+-------------+-------------+--------------+
|  Web   |             |             |              |
|sockets |      S'     |      AB'    |     ANB'     |
+--------+------------------------------------------+
```
```c++
	/* pattern S (Synchronous) */
	...
socket.waitForReadyRead(...);
... =socket.readAll();
	...
	/* pattern AB (Asynchronous Buffered) */

connect(&socket, &QUnSocket::readyRead, this, [&] () {
	... =socket.readAll();
});

	/* pattern ANB (Asynchronous Non-Buffered) */

connect(&socket, &QUnSocket::received, this, [&] (const QByteArray &data) {
	... =data;
});

	/// Receiving text data (WebSockets only) ///

	/* pattern S' (Synchronous) */
	...
socket.waitForReadyRead(...);
... =socket.readAllText();
	...
	/* pattern AB' (Asynchronous Buffered) */

connect(&socket, &QUnSocket::readyRead, this, [&] () {
	... =socket.readAllText();
});

	/* pattern ANB' (Asynchronous Non-Buffered) */

connect(&socket, &QUnSocket::receivedText, this, [&] (const QString &text) {
	... =text;
});
```

Примеры использования `QUnSocket` для всех типов сокетов находятся в тестовом проекте 
[test-socket](https://github.com/JohnMcLaren/QUnSocket/tree/main/test-socket).




























