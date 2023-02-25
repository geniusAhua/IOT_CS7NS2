import React,{Suspense} from "react"
import Card_ from "../components/card";
import "../less/homepage.less";
import Scroll from "../components/Scroll";
import Map from "../pages/map";
import Header from "../components/header";
import Footer from "../components/footer";

function HomePage(){
    return(
       <div>
           <Suspense fallback={<div>Loading...</div>}>
           <Scroll/>
           </Suspense>
           <Card_/>
       </div>
   )
}
export default HomePage;
