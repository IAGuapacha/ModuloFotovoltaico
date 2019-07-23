package com.example.modulofotovoltaico;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.LinearLayout;
import android.widget.TextView;

import java.util.ArrayList;

import static com.example.modulofotovoltaico.R.layout.adapter_sensor;

public class GridAdapter extends BaseAdapter {

    private Context context;
    private ArrayList<Sensores> dataSensores;
    private LayoutInflater mInflaterCatalogListItems;


    public GridAdapter(Context context, ArrayList<Sensores> dataSensores){
        this.context = context;
        this.dataSensores = dataSensores;
        mInflaterCatalogListItems = (LayoutInflater) context.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
    }

    @Override
    public int getCount() {
        return dataSensores.size();
    }

    @Override
    public Object getItem(int position) {
        return dataSensores.get(position);
    }

    @Override
    public long getItemId(int position) {
        return position;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup viewGroup) {

        ViewHolder holder;

        if(convertView == null){
            holder = new ViewHolder();
            convertView = mInflaterCatalogListItems.inflate(adapter_sensor,null);
            holder.Sensor = (TextView) convertView.findViewById(R.id.Sensor);
            holder.Medida = (TextView) convertView.findViewById(R.id.Medida);
            holder.linearLayout = (LinearLayout) convertView.findViewById(R.id.lyCelda);

            convertView.setTag(holder);
         } else {
            holder = (ViewHolder) convertView.getTag();
        }

        if(dataSensores.get(position) != null){
            holder.Sensor.setText(dataSensores.get(position).getSensorName());
            holder.Medida.setText(dataSensores.get(position).getMedida());
            holder.linearLayout.setBackgroundColor(dataSensores.get(position).getColor());


        }
        return convertView;
    }

    private static class ViewHolder {
        TextView Sensor;
        TextView Medida;
        LinearLayout linearLayout;
    }

}
