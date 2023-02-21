import Login from '../pages/Login'
import HomePage from '../pages/HomePage'
import Register from '../pages/Register'
import GoogleMap from '../pages/GoogleMap'
import React, {ReactNode} from 'react';
import {BrowserRouter as Router, Route, Routes} from "react-router-dom";

type BaseRouterProps = {
    children: ReactNode;

}

const BaseRouter =({ children }: BaseRouterProps)=>(
    <Router>
        <Routes>
            {/*<Route  path="/" element={<Example/>}/>*/}
            <Route  path="/login" element={<Login/>}/>
                <Route  path="/homepage" element={<HomePage/>}/>
                <Route  path="/googleMap" element={<GoogleMap/>}/>
            <Route  path="/Register" element={<Register/>}/>
        </Routes>
    </Router>
)

export default BaseRouter
