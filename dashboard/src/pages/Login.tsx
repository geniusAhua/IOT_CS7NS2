import {NumberKeyboard, VirtualInput, PasscodeInput,Button} from 'antd-mobile'
import {Trail} from '../components/demos-util'
import BasicDemo from '../components/image'
import '../less/Login.less'
import {useNavigate} from "react-router-dom";
import React, { useState } from 'react'
import './login.css'


export default function Login(){
    const navigate = useNavigate()
    const [open, set] = useState(true)
    return(
        <div>
            <div className="container"  onClick={() => set(state => !state)}>
                <Trail open={open}>
                    <span style={{color:"white"}}>Welcome</span>
                    <span style={{color:"white"}}>to</span>
                    <span style={{color:"white"}}>Smart</span>
                    <span style={{color:"white"}}>Bin</span>
                </Trail>
            </div>
            {open === false?
                <div style={{marginTop:"-55vh"}}>
                    <div style={{marginLeft: '50%', marginRight: '50%', transform: "translate(-50px,0)"}}>
                        <BasicDemo/>
                    </div>
                    <div>
                    <h2 style={{color:"white",fontWeight:"bold"}}>username:</h2>
                    <VirtualInput placeholder='please input your name' keyboard={<NumberKeyboard/>}/>
                    <h2 style={{color:"white",fontWeight:"bold"}}>password:</h2>
                    <PasscodeInput keyboard={<NumberKeyboard/>}/>
                    </div>
                    <Button style={{marginTop: '10vh',color:"white",left:"40%",right:"50%",transform:"(-200px,0)"}}
                            onClick={() => navigate('../homepage')}>
                        <p style={{color:"black",height:"0vh"}}>Login</p>
                    </Button>
                </div>
                :null
            }

        </div>
    )
}
