import request from './request'

export const RegisterApi = (params) => request.post('/user/account/register', params)

export const BinDataApi = (params) => request.get('/bin/info', params)

export const LoginApi = (params) => request.get('/user/account/login', params)