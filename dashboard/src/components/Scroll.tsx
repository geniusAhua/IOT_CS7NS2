import "./scroll.css";
import React, {useRef, Suspense} from "react";
import {motion, useScroll} from "framer-motion";
import Open_bin from "../bin/open_bin.jpg";
import Close_bin from "../bin/close_bin.jpg";
import Press_bin from "../bin/press_bin.jpg";
import {RootState} from '../store/index';
import {useSelector} from 'react-redux'

const Spinner = () => {
    return(
        <div className="loader">
            <div></div>
            <div></div>
            <div></div>
         </div>
    )
}

export default function Scroll() {
    const imgs: string[][] = [
        [Close_bin, Open_bin, Close_bin, Open_bin],
        [Open_bin, Close_bin, Open_bin, Close_bin],
        [Open_bin, Press_bin, Open_bin, Press_bin]
    ]
    const ref = useRef(null);
    const {scrollXProgress} = useScroll({container: ref});
    let dom = document.getElementById('scroll')
    console.log(dom)
    // @ts-ignore
    return (
        <div>
            <svg id="progress" width="40" height="40" viewBox="0 0 100 100 ">
                <circle cx="50" cy="50" r="30" pathLength="1" className="bg"/>
                <motion.circle
                    cx="50"
                    cy="50"
                    r="30"
                    pathLength="1"
                    className="indicator"
                    style={{pathLength: scrollXProgress}}
                />
            </svg>
            <div className="hello">
                <ul ref={ref} id="scroll" style={{position:"relative",top:"42%"}}>
                    {
                        imgs[useSelector((state: RootState) => state.user.value)].map((key, item) => {
                            return (
                                <li key={item}>
                                    <div style={{width: "33vw", height: "20vh",position:"absolute",top:"0%",left:"100%"}}><Spinner/></div>
                                    <img src={key} alt={imgs[0][0]}
                                                    style={{width: "33vw", height: "20vh"}}/></li>
                            )
                        })
                    }
                </ul>
            </div>
        </div>
    );
}
