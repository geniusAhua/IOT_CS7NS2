import React from "react"
import { Example } from "../components/Example";
import "../less/homepage.less";
import Scroll from "../components/Scroll";
import Map from "../pages/map";

function HomePage(){

    return(
       <div style={{marginLeft:'5vw'}}>
           <Example/>
           <div style={{marginTop:"2vh"}}>
           <Scroll/>
           </div>
           <Map/>
       </div>
   )
}
export default HomePage;
