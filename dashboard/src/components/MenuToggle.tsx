import * as React from "react";
import { motion } from "framer-motion";
import "../less/homepage.less"
import earth from "../bin/map.png"
import {useNavigate} from "react-router-dom";

const Path = (props:any) => (
    <motion.path
        fill="transparent"
        strokeWidth="3"
        stroke="hsl(0, 0%, 18%)"
        strokeLinecap="round"
        {...props}
    />
);

// eslint-disable-next-line react-hooks/rules-of-hooks
// @ts-ignore
export const MenuToggle = ({ toggle }) => (
    <div style={{display:"flex",height:"20vh"}}>
    <a href='../googleMap'><img src={earth} alt="null" style={{width:"30vw",height:"20vh"}}/></a>
    <button onClick={toggle} style={{float:"right",marginLeft:"55vw",height:"4vh"}} className="button">
        <svg width="23" height="23" viewBox="0 0 23 23">
            <Path
                variants={{
                    closed: { d: "M 3 2 L 20 2.5" },
                    open: { d: "M 3 20 L 17 2.5" }
                }}
            />
            <Path
                d="M 2 9.423 L 20 9.423"
                variants={{
                    closed: { opacity: 1 },
                    open: { opacity: 0 }
                }}
                transition={{ duration: 0.1 }}
            />
            <Path
                variants={{
                    closed: { d: "M 2 16.346 L 20 16.346" },
                    open: { d: "M 3 2.5 L 17 16.346" }
                }}
            />
        </svg>
    </button>
    </div>
);
