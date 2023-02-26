import React,{useState} from 'react'
import BasicDemo from "../components/image";
import {Button, NumberKeyboard, PasscodeInput, VirtualInput} from "antd-mobile";
import {useNavigate,Link} from "react-router-dom";
import { Form, Input } from 'antd-mobile'

export default function Username(){
    const navigate = useNavigate()
    const [value, setValue] = useState('')

    return(
        <div style={{width:"90%",height:"90%",marginTop:"5vh",marginLeft:"5vw",backdropFilter:"blur(16px) saturate(180%)",backgroundColor:"rgba(255, 255, 255, 0.75)",borderRadius:"12px",border:"1px solid rgba(209, 213, 219, 0.3)"}}>
            <BasicDemo/>
            <Form requiredMarkStyle='asterisk' style={{width:"90%",position:"relative",left:"5%",top:"5%"}}>
                <Form.Item name='name' label='username' rules={[{ required: true }]} style={{height:"10vh"}}>
                    <Input style={{fontSize:"16px"}} placeholder='please input your name' />
                </Form.Item>
                <Form.Item name='address' label='password' rules={[{ required: true }]} style={{height:"50%"}}>
                    <PasscodeInput keyboard={<NumberKeyboard />}/>
                </Form.Item>
            </Form>
            <Link to="./home/googleMap">
                <Button style={{marginTop: '10vh',left:"14vw",height:"5.8vh",width:"25vw",borderRadius:"5"}}>login</Button>
            </Link>
            <Link to="../home/reegister">
                <Button style={{marginTop: '10vh',left:"26vw",height:"5.8vh",width:"25vw",borderRadius:"5"}}>register</Button>
            </Link>
        </div>
    )
}
