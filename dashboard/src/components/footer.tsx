import * as React from 'react';
import BottomNavigation from '@mui/material/BottomNavigation';
import BottomNavigationAction from '@mui/material/BottomNavigationAction';
import RestoreIcon from '@mui/icons-material/Restore';
import HomeIcon from '@mui/icons-material/Home';
import LocationOnIcon from '@mui/icons-material/LocationOn';
import { useNavigate } from 'react-router-dom';

export default function Footer() {
    const [value, setValue] = React.useState('recents');

    const handleChange = (event: React.SyntheticEvent, newValue: string) => {
        setValue(newValue);
    };
    const navigate = useNavigate()
    let width = screen.width
    return (
        <BottomNavigation sx={{ width:{width},height:48}} value={value} onChange={handleChange}>
            <BottomNavigationAction label="Recents" value="recents" icon={<RestoreIcon />} style={{height:'8vh',scale:'0.5',fontSize:"5px",position:"relative",left:"1%"}}/>
            <BottomNavigationAction label="Home" value="home" icon={<HomeIcon/>}style={{height:'8vh',scale:'0.5',fontSize:"5px",position:"relative"}}/>
            <BottomNavigationAction label="Location" value="location" onClick={()=>navigate('../googleMap')} icon={<LocationOnIcon />}style={{height:'8vh',scale:'0.5',fontSize:"5px"}}/>
        </BottomNavigation>
    );
}
