To scan for soospy camera:
- try to connect to port 80 on all available hosts
- look for 'Server: IPCamera-Web' header

$ wget -S  http://192.168.1.27
--2020-09-06 19:48:09--  http://192.168.1.27/
Connecting to 192.168.1.27:80... connected.
HTTP request sent, awaiting response... 
  HTTP/1.1 401 Unauthorized
  Server: IPCamera-Web
  Date: Thu Jan  1 08:09:57 1970
  WWW-Authenticate: Basic realm="IPCamera_Web"
  Pragma: no-cache
  Cache-Control: no-cache
  Content-Type: text/html

