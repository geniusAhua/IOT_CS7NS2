import React from "react";
import {FreeCamera, Vector3, HemisphericLight, MeshBuilder, StandardMaterial, Color3} from "@babylonjs/core";
import SceneComponent from "./SceneComponent"; // uses above component in same directory
import "../less/bin.less";
import GUI from "babylonjs-gui";
import { BinDataApi } from '../request/api';

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
let params={userId:1}
let weight:number
let height:number
BinDataApi({params}).then(res=>{
        let info = JSON.parse(JSON.stringify(res))
        weight = info.weight
        height = info.height
    }).catch(function(err){
        console.log(err)
    })

let ws = new WebSocket('ws://localhost:8080/websocket/client');
// 在客户端与服务端建立连接后触发
ws.onopen = function() {
    console.log("Connection open.");
    ws.send('hello');
};
// 在服务端给客户端发来消息的时候触发
ws.onmessage = function(res) {
    console.log(res);       // 打印的是MessageEvent对象
    console.log(res.data);  // 打印的是收到的消息
};

export default function Bin() {
    return (
        <div style={{width:"100%",height:"0%"}}>
            <SceneComponent antialias onSceneReady={onSceneReady} onRender={onRender} id="my-canvas" style={{width:"100%",height:"60vh",marginTop:"5vh"}}/>
            <h2 style={{position:"relative",left:"20%",marginTop:"-10vh"}}>Weight:  {weight}</h2>
            <h2 style={{position:"relative",left:"20%"}}>Height:   {height}</h2>
        </div>
    )
}
