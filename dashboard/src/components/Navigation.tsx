import * as React from "react";
import { motion } from "framer-motion";
import { MenuItem } from "./MenuItem";

const variants = {
    open: {
        transition: { staggerChildren: 0.07, delayChildren: 0.2 },
        height:'0vh',
    },
    closed: {
        transition: { staggerChildren: 0.05, staggerDirection: -1 },
        height:'0vh',
    }
};

export const Navigation = () => (
    <div>
    <motion.ul variants={variants} >
        {itemIds.map(i => (
            <MenuItem i={i} key={i} />
        ))}
    </motion.ul>
    </div>
);

const itemIds = [0, 1, 2];
