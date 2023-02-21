import React from "react";
import { motion } from "framer-motion";
import { List} from 'antd-mobile';
import { UnorderedListOutline } from 'antd-mobile-icons'
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
        <div>
        <motion.li
            variants={variants}
            whileHover={{ scale: 1.02 }}
            whileTap={{ scale: 0.95 }}
        >
            <List style={{width:'50vw'}}>
                <List.Item prefix={<UnorderedListOutline />} onClick={() => { // @ts-ignore
                    event({params: {i}})}} style={style}>
                    {name[i]}
                </List.Item>
            </List>
        </motion.li>
        </div>
    );
};


