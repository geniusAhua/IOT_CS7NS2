const {createProxyMiddleware} = require('http-proxy-middleware');

module.exports = function(app) {
    app.use(createProxyMiddleware('/', { target: 'http://localhost:8080/' }));
};


const proxy = require('http-proxy-middleware');

module.exports = function(app) {
    app.use(proxy.createProxyMiddleware('/api', { target: 'http://localhost:8080/' }));
};
