const CACHE_NAME = "version-1";
const urlsToCache = [ 'index.html', 'offline.html' ];

const self = this;

// 具有离线性
self.addEventListener('install', (event) => {
    event.waitUntil(
        caches.open(CACHE_NAME)
            .then((cache) => {
                console.log('Opened cache');

                return cache.addAll(urlsToCache);
            })
    )
});

//具有可安装性
self.addEventListener('fetch', function (e) {
    console.log('Service Worker 状态： fetch');
    var cacheMatchPromise = caches.match(e.request).then(function (cache) {
        // 如果有cache则直接返回，否则通过fetch请求
        return cache || fetch(e.request);
    }).catch(function (err) {
        console.log(err);
        return fetch(e.request);
    })
    e.respondWith(cacheMatchPromise);
});

// 监听 activate 事件，清除缓存
self.addEventListener('activate', function (e) {
    console.log('Service Worker 状态： activate');
    var cachePromise = caches.keys().then(function (keys) {
        return Promise.all(keys.map(function (key) {
            if (key !== cacheName) {
                return caches.delete(key);
            }
        }));
    })
    e.waitUntil(cachePromise);
    return self.clients.claim();
});
