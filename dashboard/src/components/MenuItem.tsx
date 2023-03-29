import React from "react";
import { motion } from "framer-motion";
import {Card, List} from 'antd-mobile';
import {AntOutline, RightOutline} from 'antd-mobile-icons'
import { useDispatch } from 'react-redux'
import { incrementByAmount } from '../store/reducer'

const variants = {
    open: {
        y: 50,
        opacity: 1,
        transition: {
            y: { stiffness: 1000, velocity: -100 }
        },
        marginLeft:160
    },
    closed: {
        y: 80,
        opacity: 0,
        transition: {
            y: { stiffness: 1000 }
        },
        marginLeft:160
    }
};


const colors = ["#ee78a1", "#e47aec", "#c585f1"];
const name = ["bin open","bin close","need press"];
type eventProps={
    params:number
}

let move= function():void {
    // @ts-ignore
    document.getElementById('scroll').scrollLeft += 10
}

let num:number
let timer1: string | number | NodeJS.Timer | undefined;
// @ts-ignore
export const MenuItem = ({ i }) => {
    // @ts-ignore
    const dispatch = useDispatch()
    const event = ({params}: eventProps) => (
        num = Number(Object.values(params)),
        dispatch(incrementByAmount(num)),

        setTimeout(()=> {
            // @ts-ignore
            document.getElementById('scroll').scrollLeft = 0
            // eslint-disable-next-line @typescript-eslint/no-unused-expressions
                timer1 = setInterval(() => move(), 100),
                    setTimeout(() => {
                        clearInterval(timer1);
                        alert('bin has been open/closed');
                    }, 4000)
            },100
        )
    )
    const style = { border: `2px solid ${colors[i]}`,backgroundColor:`${colors[i]}`};
    // @ts-ignore
    return (
        <div style={{listStyleType:"none",background: "rgba( 255, 245, 245, 0.85 )",boxShadow:"0 8px 32px 0 rgba( 31, 38, 135, 0.37 )",backdropFilter:"blur( 2px )",width:"80vw"}}>
        <motion.li
            variants={variants}
            whileHover={{ scale: 1.02 }}
            whileTap={{ scale: 0.95 }}
        >
            <List style={{width:'80vw'}}>
                <Card
                    title={
                        <div style={{ fontWeight: 'normal' }}>
                            <AntOutline style={{ marginRight: '4px', color: '#1677ff' }} />
                            {name[i]}
                        </div>
                    }
                    extra={<RightOutline />}
                    style={{ borderRadius: '16px' }}
                    onClick={() => { // @ts-ignore
                        event({params: {i}})}}
                >
                </Card>
            </List>
        </motion.li>
        </div>
    );
};


