import "./login.css";
import { motion } from "framer-motion";

const draw = {
    hidden: { pathLength: 0, opacity: 0 },
    visible: (i: number) => {
        const delay = i * 0.2;
        return {
            pathLength: 1,
            opacity: 1,
            transition: {
                pathLength: { delay, type: "spring", duration: 0.5, bounce: 0 },
                opacity: { delay, duration: 0 }
            }
        };
    }
};

export default function Login_box() {
    return (
        <motion.svg
            width="600"
            height="650"
            viewBox="0 0 600 600"
            initial="hidden"
            animate="visible"
        >
            <motion.line
                x1="120"
                y1="230"
                x2="120"
                y2="370"
                stroke="#b8a1cf"
                custom={1}
                variants={draw}
            />
            <motion.line
                x1="120"
                y1="230"
                x2="260"
                y2="370"
                stroke="#b8a1cf"
                custom={2}
                variants={draw}
            />
        </motion.svg>
    );
}
