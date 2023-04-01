/**
 * This is an auto-generated demo by dumi
 * if you think it is not working as expected,
 * please report the issue at
 * https://github.com/umijs/dumi/issues
 **/

import React from 'react'
import { LoremIpsum } from 'lorem-ipsum'
import {a, useTrail} from "@react-spring/web";
import  '../pages/login.css'

export const lorem = new LoremIpsum({
    sentencesPerParagraph: {
        max: 8,
        min: 4,
    },
    wordsPerSentence: {
        max: 16,
        min: 4,
    },
})

// @ts-ignore
export const DemoBlock = ({ title, children,background}) => (
    <div style={{ background }}>
        <h3>{title}</h3>
        {children}
    </div>
);

// @ts-ignore
export const DemoDescription = ({ children }) => <div style={{ opacity: 0.5 }}>{children}</div>;

export const sleep = (time: number) => new Promise(resolve => setTimeout(resolve, time))

// @ts-ignore
export const Trail = ({ open, children }) => {
    const items = React.Children.toArray(children)
    const trail = useTrail(items.length, {
            config: {mass: 5, tension: 2000, friction: 200},
            opacity: open ? 1 : 0,
            x: open ? 0 : 20,
            height: open ? 110 : 0,
            from: {opacity: 0, x: 20, height: 0},
        })
    return (
        <div>
            {trail.map(({height, ...style}, index) => (
                <a.div key={index} className="trailsText" style={style}>
                    <a.div style={{height}}>{items[index]}</a.div>
                </a.div>
            ))}
        </div>
    )
}
