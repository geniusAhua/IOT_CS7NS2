const {createProxyMiddleware} = require('http-proxy-middleware');

module.exports = function(app) {
    app.use(createProxyMiddleware('/', { target: 'http://localhost:3000/' }));
};


const proxy = require('http-proxy-middleware');

module.exports = function(app) {
    app.use(proxy.createProxyMiddleware('/api', { target: 'http://localhost:3000/' }));
};
