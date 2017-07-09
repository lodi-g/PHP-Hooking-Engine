# PHP-Hooking-Engine

Based on [Paradoxis work](https://github.com/Paradoxis/PHP-Rootkit).

### Description
An hooking engine for PHP: allows anyone to hook a builtin PHP function or class method to replace it with his own.

### Requirements
* PHP v7.1.6 (probably works for PHP >= 7, not tested)

### Compiling
* `phpize`
* `./configure`
* `make`
* `sudo make install`
* Enable the new extension (default name: he)

### Testing
* `php -r 'echo zend_version();'`
* `php -r 'echo phpversion();'`
* `php -r '$dbh = new PDO("mysql:host=localhost;dbname=db", "user", "pass");'`
* These are only the tests I used to test the engine. You can add as many as you want.