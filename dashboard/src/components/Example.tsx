import * as React from "react";
import { useRef } from "react";
import { motion, useCycle } from "framer-motion";
import { useDimensions } from "./use-dimensions";
import { MenuToggle } from "./MenuToggle";
import { Navigation } from "./Navigation";
import '../less/homepage.less'

const sidebar = {
    open: (height = 1000) => ({
        clipPath: `circle(${height * 2 + 300}vw at 40vw 40vw)`,
        transition: {
            type: "spring",
            stiffness: 20,
            restDelta: 2
        }
    }),
    closed: {
        clipPath: "circle(30vm at 40vm 40vm)",
        transition: {
            delay: 0.5,
            type: "spring",
            stiffness: 400,
            damping: 40
        }
    }
};

export const Example = () => {
    const [isOpen, toggleOpen] = useCycle(false, true);
    const containerRef = useRef(null);
    const { height } = useDimensions(containerRef);

    return (
        <div className="welcome">
        <motion.nav
            initial={false}
            animate={isOpen ? "open" : "closed"}
            custom={height}
            ref={containerRef}
        >
            <motion.div className="background" variants={sidebar} />
            <Navigation/>
            <MenuToggle toggle={() => toggleOpen()} />
        </motion.nav>
        </div>
    );
};
