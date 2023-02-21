import React, { useEffect, useRef } from 'react'
import { FloatingPanel } from 'antd-mobile'
import Bin from "../pages/bin";

const anchors = [100, window.innerHeight * 0.4, window.innerHeight * 1]
const minHeight = anchors[0]
const maxHeight = anchors[anchors.length - 1]

export default () => {
    const targetRef = useRef<HTMLDivElement>(null)

    const onHeightChange = (height: number) => {
        const ratio = height / maxHeight
        console.log(ratio)
        const target = targetRef.current
        if (!target) return
        target.style.height = '100%'
        target.style.backgroundImage = `linear-gradient(rgba(185,147,214,${ratio}),rgba(140,166,219,${ratio}))`
    }

    useEffect(() => {
        onHeightChange(minHeight)
    }, [])

    return (
        <div
            style={{
                padding: 12,
            }}
        >
            <FloatingPanel anchors={anchors} onHeightChange={onHeightChange}>
                <h1 style={{color:"gray"}}>3D BIN</h1>
                <Bin/>
                <div style={{color:"black",marginLeft:"2vw",fontWeight:"bold"}}>
                    <h2 style={{color:"black"}}>weight:</h2>
                    <h2 style={{color:"black"}}>height:</h2>
                </div>
                <div ref={targetRef} />
            </FloatingPanel>
        </div>
    )
}
