import * as React from 'react';
import BottomNavigation from '@mui/material/BottomNavigation';
import BottomNavigationAction from '@mui/material/BottomNavigationAction';
import ThreeDRotationIcon from '@mui/icons-material/ThreeDRotation';
import HomeIcon from '@mui/icons-material/Home';
import LocationOnIcon from '@mui/icons-material/LocationOn';
import { useNavigate } from 'react-router-dom';

export default function Footer() {
    const [value, setValue] = React.useState('home');
    
    const handleChange = (event: React.SyntheticEvent, newValue: string) => {
        setValue(newValue);
    };
    if(value =="location") {
        setValue("home");
    }

    const navigate = useNavigate()
    let width = screen.width
    return (
        <BottomNavigation sx={{ width:{width},height:48}} value={value} onChange={handleChange}>
            <BottomNavigationAction label="3D" value="3D" icon={<ThreeDRotationIcon />} onClick={()=>navigate('./bin')} style={{height:'8vh',scale:'0.5',fontSize:"5px",position:"relative",left:"1%"}}/>
            <BottomNavigationAction label="Home" value="home" icon={<HomeIcon/>} onClick={()=>navigate('./homepage')} style={{height:'8vh',scale:'0.5',fontSize:"5px",position:"relative"}}/>
            <BottomNavigationAction label="Location" value="location" onClick={()=>navigate('./googleMap')} icon={<LocationOnIcon />}style={{height:'8vh',scale:'0.5',fontSize:"5px"}}/>
        </BottomNavigation>
    );
}
