# About
Software for testing connection speed initiated by zabbix.

The util downloads file from remote HTTP (not HTTPS) server and measures speed during download. It gathering max, min and avg speed metrics with quantification of 100ms, 500ms and 1000ms that gives you more complex information about channel quality.

# Input parameters
First parameter is URL with file. Unfortunnaly, possible protocol is only HTTP, HTTPS is not supported.
Second parameter is download duration in seconds.

# Output
Output is formatted as JSON array especially for Zabbix, e.g:
```
{"100ms":{"min":10, "max":20, "avg":15}, "500ms":{"min":10, "max":20, "avg":15}, "1s":{"min":10, "max":20, "avg":15}}
```