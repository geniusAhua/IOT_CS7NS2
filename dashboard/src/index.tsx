import React from 'react';
import ReactDOM from 'react-dom/client';
import './index.css';
import App from './App';
import 'lib-flexible';
import reportWebVitals from './reportWebVitals';
import BaseRouter from './router';
import { Provider } from 'react-redux';
import store from './store/index';

const root = ReactDOM.createRoot(
  document.getElementById('root') as HTMLElement
);

root.render(
      <React.StrictMode>
          <Provider store={store}>
          <BaseRouter>
               <App />
          </BaseRouter>
          </Provider>
       </React.StrictMode>
        );

reportWebVitals();
