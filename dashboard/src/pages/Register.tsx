import React, {useState,useCallback} from 'react'
import BasicDemo from "../components/image";
import {Button, NumberKeyboard, PasscodeInput, VirtualInput} from "antd-mobile";
import {useNavigate, Link} from "react-router-dom";
import {Form, Input} from 'antd-mobile'
import Background from 'smart-background';
import { RegisterApi } from '../request/api';
import axios from 'axios';

export default function Register(){
    const symbols:number[] = []
    const navigate = useNavigate()
    const [form, setForm] = useState({name:'',password:'',confirmPassword:''})
    const handleChange = useCallback((val:any, name:any) => {
        setForm(preVal => ({ ...preVal, [name]: val }))
    }, [])

    const setCookie = (name:any, value:any, expiryDate:any) => {
        let currentDate = new Date();
        currentDate.setDate(currentDate.getDate() + expiryDate);
        document.cookie = name + '=' + value + '; expires=' + currentDate ;
    };
    

    const getCookie = (name:any) => {
        let arr = document.cookie.split('; ');
        for (let i = 0; i < arr.length; i++) {
            let arr2 = arr[i].split('=');
            if (arr2[0] === name) {
                return arr2[1];
            }
        }
        return '';
    };

    const removeCookie = (name:any) => {
        setCookie(name, 1, -1);
    };

    const submit=()=> {
        if (form.password !== '' && form.name !== '') {
            setCookie('username', form.name, 1);
            setCookie('password', form.password, 1);
            setCookie('confirmPassword', form.confirmPassword, 1);
  
            axios({
                method: 'post',
                url: '/api/user/account/register',
                data: {
                    username:form.name,
                    password:form.password,
                    confirmPassword:form.confirmPassword
                }
            })
                .then(function (response) {
                    if(response.data.error_message == 'success'){
                        navigate('../')
                    }
                });
            
        } else {
            alert("userName and password cannot be none")
        }
    }
    
    return (
        <Background
            underlayImage='linear-gradient(to right, #fffff0 0%,#a9a9a9 100%)'
            symbolsStyle={{color:'rgba(45,45,45,0.8)'}}
            symbolSize={20}
            gap={20}
            animation={{ type: 'right', speed: 5 }}
            rotate={45}
            symbols={symbols}
        >
        <div style={{
            width: "90%",
            height: "90%",
            marginTop: "5vh",
            marginLeft: "5vw",
            backdropFilter: "blur(16px) saturate(180%)",
            backgroundColor: "rgba(255, 255, 255, 0.75)",
            borderRadius: "12px",
            border: "1px solid rgba(209, 213, 219, 0.3)"
        }}>
            <BasicDemo/>
            <Form requiredMarkStyle='asterisk' style={{width: "90%", position: "relative", left: "5%", top: "5%"}}>
                <Form.Item name='name' label='username' rules={[{required: true}]} style={{height: "10vh"}}>
                    <Input style={{fontSize: "16px"}} placeholder='please input your name' value={form.name}
                           onChange={(val) => handleChange(val,'name')}/>
                </Form.Item>
                <Form.Item name='address' label='password' rules={[{required: true}]} style={{height: "50%"}}>
                    <PasscodeInput keyboard={<NumberKeyboard/>} value={form.password}
                                   onChange={(val) => (val.length==6)&&handleChange(val,'password')}/>
                </Form.Item>
                <Form.Item name='password' label='confirmPassword' rules={[{required: true}]} style={{height: "50%"}}>
                    <PasscodeInput keyboard={<NumberKeyboard/>} value={form.confirmPassword}
                                   onChange={(val) => (val.length==6)&&handleChange(val,'confirmPassword')}/>
                </Form.Item>
            </Form>
            <Button style={{
                marginTop: '8vh',
                left: "36%",
                height: "5.8vh",
                width: "25vw",
                borderRadius: "5"
            }} onClick={submit}>submit</Button>
            <h4 style={{
                marginTop: '6vh',
                marginLeft: "5vw"
            }}>Register success?<a onClick={()=>navigate(-1)}>Please return login</a></h4>
        </div>
        </Background>
    )
}
