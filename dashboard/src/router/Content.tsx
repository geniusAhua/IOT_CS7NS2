import React, { useState, useEffect } from "react";
import {
    BrowserRouter,
    Link,
    useLocation,
    Route,
    Routes
} from "react-router-dom";
import Login from "../pages/Login";
import HomePage from "../pages/HomePage";
import GoogleMap from "../pages/GoogleMap";
import Register from "../pages/Register";
import "../less/router.css";
import App from "../App";

export default function Content() {
    const location = useLocation();

    const [displayLocation, setDisplayLocation] = useState(location);
    const [transitionStage, setTransistionStage] = useState("fadeIn");

    useEffect(() => {
        if (location !== displayLocation) setTransistionStage("fadeOut");
        console.log(displayLocation,transitionStage)
    }, [location, displayLocation]);

    return (
        <div
            className={`${transitionStage}`}
            onAnimationEnd={() => {
                // @ts-ignore
                if (transitionStage === "fadeOut") {
                    setTransistionStage("fadeIn");
                    setDisplayLocation(location);
                }
            }}
        >
            <Routes location={displayLocation}>
                <Route path="/" element={<App/>}/>
                <Route path="/homepage" element={<HomePage/>}/>
                <Route path="/googleMap" element={<GoogleMap/>}/>
                <Route path="/Register" element={<Register/>}/>
            </Routes>
        </div>
    );
}
