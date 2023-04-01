import {NumberKeyboard, VirtualInput, PasscodeInput,Button} from 'antd-mobile'
import {Trail} from '../components/demos-util'
import '../less/Login.less'
import React, { useState } from 'react'
import SVG from "./svg_container";
import Username from "./Username";
import Background from 'smart-background';

const symbols: number[] =  []
export default function Login(){
    const [open, set] = useState(true)
    let flag = true

    // @ts-ignore
    if(localStorage.getItem('count')==1){
        flag = false
    }
    return(
        <div>
            {open === false || flag == false?
                <Background
                    underlayImage='linear-gradient(to right, #fffff0 0%,#a9a9a9 100%)'
                    symbolsStyle={{color:'rgba(45,45,45,0.8)'}}
                    symbolSize={20}
                    gap={20}
                    animation={{ type: 'right', speed: 5 }}
                    rotate={45}
                    symbols={symbols}
                >
                <Username/>
                </Background>
                :
                <div>
                    <SVG></SVG>
                <div onClick={() => set(false)} style={{position:"absolute",right:"0",top:"35%",bottom:"0",left:"45%"}}>
                    <Trail open={open}>
                    <span>Hi</span>
                     </Trail>
                </div>
                </div>
            }
        </div>
    )
}
