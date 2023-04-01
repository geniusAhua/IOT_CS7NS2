import request from './request'

export const RegisterApi = (params:any) => request.post('/user/account/register', params)

export const BinDataApi = (params: any) => request.get('/bin/info', params)

export const LoginApi = (params:any) => request.post('/user/account/login', params)

export const PostApi = (params:any) => request.post('/publish',params)