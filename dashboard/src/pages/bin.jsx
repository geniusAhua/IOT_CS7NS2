import React, { useState, useRef, useCallback, useEffect } from 'react';
import {FreeCamera, Vector3, HemisphericLight, MeshBuilder, StandardMaterial, Color3} from "@babylonjs/core";
import SceneComponent from "./SceneComponent"; // uses above component in same directory
import "../less/bin.less";
import GUI from "babylonjs-gui";
import { BinDataApi } from '../request/api';
import { UilTemperature, UilWater, UilBell, UilHourglass } from '@iconscout/react-unicons'
import { UilTrashAlt } from '@iconscout/react-unicons'

let sphere;
let cylinder;

const onSceneReady = (scene) => {
    // This creates and positions a free camera (non-mesh)
    const camera = new FreeCamera("camera1", new Vector3(0, 5, -10), scene);
    scene.clearColor = new Color3(1, 1, 1);
    // This targets the camera to scene origin
    camera.setTarget(Vector3.Zero());

    const canvas = scene.getEngine().getRenderingCanvas();

    // This attaches the camera to the canvas
    camera.attachControl(canvas, true);

    // This creates a light, aiming 0,1,0 - to the sky (non-mesh)
    var light = new HemisphericLight("hemiLight", new Vector3(-1, 1, 0), scene); //
    light.diffuse = new Color3(0.95, 0.64, 0.64); //添加灯光普通光照颜色红色
    light.specular = new Color3(0.42, 0.22, 0.22); //创建高光颜色为绿色
    light.groundColor = new Color3(0.47, 0.01, 0.23); //创建底部照射光为绿色
    var redMat = new StandardMaterial("redMat", scene);

    // Our built-in 'box' shape.
    sphere = MeshBuilder.CreateTorus("torus", {diameter: 2, segments: 10}, scene);
    cylinder = MeshBuilder.CreateCylinder("cylinder", {diameterTop:2.2, diameterBottom: 1.5, height: 2, tessellations: 16, subdivisions: 5}, scene);
    sphere.material = redMat;
    // Move the sphere upward 1/2 its height
    sphere.position.y = 2;
    cylinder.position.y = 1;

};

/**
 * Will run on every frame render.  We are spinning the box on y-axis.
 */
const onRender = (scene) => {
    if (cylinder !== undefined) {
        const deltaTimeInMillis = scene.getEngine().getDeltaTime();

        const rpm = 10;
        cylinder.rotation.y += (rpm / 60) * Math.PI * 2 * (deltaTimeInMillis / 1000);
    }
};


export default function Bin() {
    const [data, setData] = React.useState({ temperature: 36, humidity: 310, EmergencyLevel: 1 });
    const [temperature, setTemperature] = useState(null);
    const [humidity, setHumidity] = useState(null);
    const [emergencyLevel, setEmergencyLevel] = useState(null);
    const [time,setTime] = useState(null)

    useEffect(() => {
        const ws = new WebSocket('ws://localhost:8080/websocket/client');
        ws.onopen = function() {
            console.log("Connection open.");
        };
        ws.onmessage = (event) => {
            const data = JSON.parse(event.data);
            const time_change = Change_time()
            console.log(data,time)
            setTemperature(data.humiture.temperature);
            setHumidity(data.humiture.Humidity);
            setEmergencyLevel(data.EmergencyLevel);
            setTime(time_change)
        };
        return () => {
            ws.close();
        };
    }, []);

    function Change_time(){
        const now = new Date();
        const timestamp = now.getTime();
        const randomDelay = Math.floor(Math.random() * (50 - 5 + 1) + 5);
        const adjustedTimestamp = timestamp - randomDelay;
        const adjustedDate = new Date(adjustedTimestamp);
        const year = adjustedDate.getFullYear();
        const month = adjustedDate.getMonth() + 1;
        const day = adjustedDate.getDate();
        const hour = adjustedDate.getHours();
        const minute = adjustedDate.getMinutes();
        const second = adjustedDate.getSeconds();
        const time = `${year}-${month}-${day} ${hour}:${minute}:${second}`
        return time
    }


    return (
        <div style={{width: "100%", height: "100%"}}>
            <SceneComponent antialias onSceneReady={onSceneReady} onRender={onRender} id="my-canvas"
                            style={{width: "100%", height: "50vh", marginTop: "5vh",objectFit: "cover", objectPosition: "30% 50%"}}/>
            <div style={{marginTop:"-2rem"}}>
            <p style={{position: "relative", left: "20%", fontFamily: 'Knewave', fontSize:'.4rem'}}><UilTemperature style={{width:".4rem",height:".4rem",color:"#44A723",border:".2rem",border:"solid",borderRadius:"50%",marginRight:".3rem"}}/>Temperature: {temperature}</p>
            <p style={{position: "relative", left: "20%", fontFamily: 'Knewave', fontSize:'.4rem'}}><UilWater style={{width:".4rem",height:".4rem",color:"#44A723",border:".2rem",border:"solid",borderRadius:"50%",marginRight:".3rem"}}/>Humidity: {humidity}</p>
            <p style={{position: "relative", left: "20%", fontFamily: 'Knewave', fontSize:'.4rem'}}><UilBell style={{width:".4rem",height:".4rem",color:"#44A723",border:".2rem",border:"solid",borderRadius:"50%",marginRight:".3rem"}}/>EmergencyLevel: {emergencyLevel}</p>
            <p style={{position: "relative", left: "20%", fontFamily: 'Knewave', fontSize:'.4rem'}}><UilHourglass style={{width:".4rem",height:".4rem",color:"#44A723",border:".2rem",border:"solid",borderRadius:"50%",marginRight:".3rem"}}/>Time: {time}</p>
            </div>
            <img src="https://i.328888.xyz/2023/04/04/ib3y1t.png" style={{width:"100%",height:"auto",position:"relative",bottom:"0px",zIndex:"-2"}}/>
        </div>
    )
}
