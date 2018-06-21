# HTTP service

This service implement HTTP endpoint for knowledge base.
It runs as an extension module of sc-memory. There are list of provided API's:

* [REST API](rest_api.md) - standard REST API for getting of post data to knowledge base. Typicaly used to get some common data like a content of sc-link with correct mime type;
* [WebSocket](websocket.md) - websocket JSON based implementation of two side protocol, that allows to communicate with knowledge base in to directions. It allows to generate/get/search anything in KB. Also you should use it to subscribes to an events.
