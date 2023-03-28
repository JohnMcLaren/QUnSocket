## QUnSocket

Universal wrapper class, to establish connection and data to send/receive by protocols: 
`UNIX-sockets`, `HTTP`/`HTTPS`, `TCP`/`UDP`, `Websockets`.

* All code of the `QUnSocket` class is implemented in one tiny `c++` header file.
* To create a connection to a server, for any protocol other than `UNIX-sockets`, 
  `connectToServer()` method accepts a string in the format:
  `protocol://location:port`
  where, the attribute `port` is not specified if a standard port (80 / 443) is used.
  For a `UNIX-sockets` connection, a string is given in the format:
  `unix:/location`
* All protocols have a `binary channel` for sending/receiving data.
* The WebSockets protocol has an additional `text channel` for sending/receiving data.
* Actions for establishing a connection, sending and receiving data, and error handling 
  for all protocols are `identical`.
* All protocols have the ability to receive data in three ways:
  1. `Synchronous` (always buffered)
  2. `Asynchronous buffered`
  3. `Asynchronous non-buffered`
* The `WebSockets` protocol, in addition to the methods described above for receiving `binary` data, 
  has also methods for receiving `text` data.
* Only the `networks` library (`QT` += `network`) is mandatory for including to the Qt-project.
  If the project will not use sockets of type `WebSockets` then the library `websockets`
  library does `not need` to be included in the project.
* Compatible `Qt5`/`Qt6`

#### Patterns of ways to receive data:
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

Examples of using `QUnSocket` for all types of sockets can be found in the test project 
[test-socket](https://github.com/JohnMcLaren/QUnSocket/tree/main/test-socket).




























