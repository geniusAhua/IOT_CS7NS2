import {BrowserRouter as Router, Route, Routes, redirect}from "react-router-dom";
import Login from "../pages/Login";
import HomePage from "../pages/HomePage";
import GoogleMap from "../pages/GoogleMap";
import Register from "../pages/Register";
import React, {ReactNode} from "react";
import App from "../App";

type BaseRouterProps= {
    children:ReactNode
}
export default function BaseRouter(children:BaseRouterProps) {
    return(
    <Router>
    <Routes>
        <Route path="/home" element={<App/>}>
              <Route path="/home/homepage" element={<HomePage/>} />
              <Route path="/home/googleMap" element={<GoogleMap/>}/>
        </Route>
        <Route index path="/" element={<Login/>} />
        <Route path="/Register" element={<Register/>}/>
    </Routes>
    </Router>
    )

}
