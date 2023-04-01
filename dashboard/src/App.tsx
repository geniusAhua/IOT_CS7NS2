import React from 'react';
import {Routes, Route, Link, useLocation} from "react-router-dom";
import {Outlet} from 'react-router-dom';
import './App.css';
import Header from "./components/header";
import Footer from "./components/footer";

function App() {
    return (
        <div id="app">
            <div style={{position: "absolute", top: "0px", width: "100%"}}>
                <Header/>
                <div className="container_box" style={{position: "absolute", top: "100%", width: "100%"}}>
                    <Outlet/>
                </div>
            </div>
            <div style={{
                position: "absolute",
                bottom: "0px",
                width: "100%",
                background: "rgba( 255, 245, 245, 0 )",
                boxShadow: "0 8px 32px 0 rgba( 31, 38, 135, 0.17 )",
                backdropFilter: "blur( 1.5px )",
                borderRadius: "10px",
                border: "1px solid rgba( 255, 255, 255, 0.18 )"
            }}>
                <Footer/>
            </div>
        </div>
    );
}

export default App;
