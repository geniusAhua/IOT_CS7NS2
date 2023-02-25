import {BrowserRouter as Router, Route, Routes} from "react-router-dom";
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
        <Route path="/" element={<App/>}>
              <Route path="/homepage" element={<HomePage/>}/>
              <Route path="/googleMap" element={<GoogleMap/>}/>
        </Route>
        <Route path="/Login" element={<Login/>}/>
        <Route path="/Register" element={<Register/>}/>
    </Routes>
    </Router>
    )
}
